#include "smplthread.h"

#include <map>
#include <deque>
#include <mutex>
#include <cassert>

class One_Way {

    private:

        bool closed = false;
        std::deque<std::string> _msgs;
        std::mutex _msg_q_lock;

        std::condition_variable _has_msg;

    public:
        One_Way(){};
        ssize_t send(const std::string &next_msg) noexcept{
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed){
                return -1;
            }
            else{
                _msgs.push_back(next_msg);
                _has_msg.notify_one();
            }
            return next_msg.size();
        }
        std::string recv() noexcept{
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed && _msgs.empty()){
                return "";
            }
            while(_msgs.empty()){
                _has_msg.wait(l);
                if(closed && _msgs.empty()){
                    return "";
                }
                else if(closed && !_msgs.empty()){
                    break;
                }
                else if(!closed && !_msgs.empty()){
                    break;
                }
                else if(!closed && _msgs.empty()){
                    assert(false);
                }
                else{
                    assert(false);
                }
            }
            const std::string m = _msgs.front();
            _msgs.pop_front();
            return m;
        }
        bool wait() noexcept{
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed && _msgs.empty()){
                return false;
            }
            while(_msgs.empty()){
                _has_msg.wait(l);
                if(closed && _msgs.empty()){
                    return false;
                }
                else if(closed && !_msgs.empty()){
                    break;
                }
                else if(!closed && !_msgs.empty()){
                    break;
                }
                else if(!closed && _msgs.empty()){
                    assert(false);
                }
                else{
                    assert(false);
                }
            }
            return true;

        }
        void close(){
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed){
                return;
            }
            else{
                closed = true;
            }
            _has_msg.notify_all();
            return;
        }
};

class Duplex{

    public:
        std::shared_ptr<One_Way> server_receiver;
        std::shared_ptr<One_Way> client_receiver;
        Duplex(){
            server_receiver = nullptr;
            client_receiver = nullptr;
        }

};

class Waiting_Connection{

    public:
        std::mutex _m;
        std::condition_variable _c;

        Duplex connection;

        Waiting_Connection() : connection(){

            assert(connection.server_receiver == nullptr);
            assert(connection.client_receiver == nullptr);

        }

};

std::mutex connection_queues_lock;
std::map<std::thread::id, std::deque<std::shared_ptr<Waiting_Connection>>> connection_queues;

Thread_Listener::Thread_Listener(){
    _self = std::this_thread::get_id();
    std::lock_guard<std::mutex> l(connection_queues_lock);
    connection_queues[_self];
}

Thread_Listener::~Thread_Listener(){
    std::lock_guard<std::mutex> l(connection_queues_lock);
    connection_queues.erase(_self);
}

smpl::Channel* Thread_Listener::listen() noexcept{
    std::shared_ptr<One_Way> receiver;
    try{
        receiver.reset(new One_Way());
    }
    catch(...){
        return nullptr;
    }

    std::shared_ptr<One_Way> sender;
    std::shared_ptr<Waiting_Connection> next;

    {
        std::unique_lock<std::mutex> l(connection_queues_lock);
        auto *q = &(connection_queues.at(_self));
        if(q->empty()){ //No client currently blocked connecting
            try{
                next.reset(new Waiting_Connection());
            }
            catch(...){
                return nullptr;
            }
            next->connection.server_receiver = receiver;
            q->push_back(next);
        }
        else{
            next = q->front();
            next->connection.server_receiver = receiver;
            assert(next->connection.client_receiver != nullptr);
            q->pop_front();
        }
    }
    assert(next->connection.server_receiver != nullptr);
    {
        std::unique_lock<std::mutex> l(next->_m);
        if(next->connection.client_receiver == nullptr){
            //wait
            while(next->connection.client_receiver == nullptr){
                assert(next->connection.client_receiver == nullptr);
                assert(next->connection.server_receiver != nullptr);
                next->_c.wait(l);
            }
            assert(next->connection.server_receiver != nullptr);
            assert(next->connection.client_receiver != nullptr);
            sender = next->connection.client_receiver; //I think this throws no exceptions...
        }
        else{
            //signal
            assert(next->connection.client_receiver != nullptr);
            assert(next->connection.server_receiver != nullptr);
            sender = next->connection.client_receiver; //I think this throws no exceptions...
            next->_c.notify_all();
        }
    }

    assert(sender != nullptr);
    assert(receiver != nullptr);
    Thread_Channel *new_tc = nullptr;
    try{
        new_tc = new Thread_Channel(sender, receiver);
    }
    catch(...){
        return nullptr;
    }
    return new_tc;
}

bool Thread_Listener::check() noexcept{
    std::unique_lock<std::mutex> l(connection_queues_lock);
    return ( !connection_queues[_self].empty() );
}

Thread_ID::Thread_ID(const std::thread::id &peer){
    _peer = peer;
}

smpl::Channel* Thread_ID::connect() noexcept{
    std::shared_ptr<One_Way> receiver;
    try{
        receiver.reset(new One_Way());
    }
    catch(...){
        return nullptr;
    }

    std::shared_ptr<One_Way> sender;
    std::shared_ptr<Waiting_Connection> next;

    try{
        std::unique_lock<std::mutex> l(connection_queues_lock);
        auto *q = &(connection_queues.at(_peer));
        if(q->empty() || q->front()->connection.server_receiver == nullptr){ //server not blocked or we're not next in line
            try{
                next.reset(new Waiting_Connection());
            }
            catch(...){
                return nullptr;
            }
            next->connection.client_receiver = receiver;
            q->push_back(next);
        }
        else{ //server is blocked listening and we're next
            next = q->front();
            next->connection.client_receiver = receiver;
            assert(next->connection.server_receiver != nullptr);
            q->pop_front();
        }
    }
    catch(std::out_of_range o){
        return nullptr;
    }
    assert(next->connection.client_receiver != nullptr);
    {
        std::unique_lock<std::mutex> l(next->_m);
        if(next->connection.server_receiver == nullptr){
            //wait
            while(next->connection.server_receiver == nullptr){
                assert(next->connection.server_receiver == nullptr);
                assert(next->connection.client_receiver != nullptr);
                next->_c.wait(l);
            }
            assert(next->connection.client_receiver != nullptr);
            assert(next->connection.server_receiver != nullptr);
            sender = next->connection.server_receiver;
        }
        else{
            //signal
            assert(next->connection.server_receiver != nullptr);
            assert(next->connection.client_receiver != nullptr);
            sender = next->connection.server_receiver;
            next->_c.notify_all();
        }
    }

    assert(sender != nullptr);
    assert(receiver != nullptr);
    Thread_Channel *new_tc = nullptr;
    try{
        new_tc  = new Thread_Channel(sender, receiver);
    }
    catch(...){
        return nullptr;
    }
    return nullptr;
}

Thread_Channel::Thread_Channel(std::shared_ptr<One_Way> sender, std::shared_ptr<One_Way> receiver){
    _sender = sender;
    _receiver = receiver;
}

Thread_Channel::~Thread_Channel(){
    _receiver->close();
    _sender->close();
}

ssize_t Thread_Channel::_send(const std::string &msg) noexcept{
    return _sender->send(msg);
}

std::string Thread_Channel::recv() noexcept{
    return _receiver->recv();
}

bool Thread_Channel::wait() noexcept{
    return _receiver->wait();
}
