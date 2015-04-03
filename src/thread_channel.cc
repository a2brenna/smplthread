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
        void send(const std::string &next_msg){
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed){
                throw smpl::Error("Closed");
            }
            else{
                _msgs.push_back(next_msg);
                _has_msg.notify_one();
            }
            return;
        }
        std::string recv(){
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed && _msgs.empty()){
                throw smpl::Error("Closed");
            }
            while(_msgs.empty()){
                _has_msg.wait(l);
                if(closed && _msgs.empty()){
                    throw smpl::Error("Closed");
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
        void wait(){
            std::unique_lock<std::mutex> l(_msg_q_lock);
            if(closed && _msgs.empty()){
                throw smpl::Error("Closed");
            }
            while(_msgs.empty()){
                _has_msg.wait(l);
                if(closed && _msgs.empty()){
                    throw smpl::Error("Closed");
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
            return;

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

smpl::Channel* Thread_Listener::listen(){
    std::shared_ptr<One_Way> receiver(new One_Way());
    std::shared_ptr<One_Way> sender;
    std::shared_ptr<Waiting_Connection> next;

    {
        std::unique_lock<std::mutex> l(connection_queues_lock);
        auto *q = &(connection_queues.at(_self));
        if(q->empty()){ //No client currently blocked connecting
            next = std::shared_ptr<Waiting_Connection>(new Waiting_Connection());
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
            sender = next->connection.client_receiver;
        }
        else{
            //signal
            assert(next->connection.client_receiver != nullptr);
            assert(next->connection.server_receiver != nullptr);
            sender = next->connection.client_receiver;
            next->_c.notify_all();
        }
    }

    assert(sender != nullptr);
    assert(receiver != nullptr);
    return (new Thread_Channel(sender, receiver));
}

bool Thread_Listener::check(){
    std::unique_lock<std::mutex> l(connection_queues_lock);
    return ( !connection_queues[_self].empty() );
}

Thread_ID::Thread_ID(const std::thread::id &peer){
    _peer = peer;
}

smpl::Channel* Thread_ID::connect(){
    std::shared_ptr<One_Way> receiver(new One_Way());
    std::shared_ptr<One_Way> sender;
    std::shared_ptr<Waiting_Connection> next;

    try{
        std::unique_lock<std::mutex> l(connection_queues_lock);
        auto *q = &(connection_queues.at(_peer));
        if(q->empty() || q->front()->connection.server_receiver == nullptr){ //server not blocked or we're not next in line
            next = std::shared_ptr<Waiting_Connection> (new Waiting_Connection());
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
        throw smpl::Error("No listening thread");
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
    return (new Thread_Channel(sender, receiver));
}

Thread_Channel::Thread_Channel(std::shared_ptr<One_Way> sender, std::shared_ptr<One_Way> receiver){
    _sender = sender;
    _receiver = receiver;
}

Thread_Channel::~Thread_Channel(){
    _receiver->close();
    _sender->close();
}

void Thread_Channel::send(const std::string &msg){
    _sender->send(msg);
    return;
}

std::string Thread_Channel::recv(){
    return _receiver->recv();
}

void Thread_Channel::wait(){
    _receiver->wait();
}
