#include "thread_channel.h"

#include <map>
#include <deque>
#include <mutex>

class One_Way {

    private:

        bool closed = false;
        std::deque<std::string> _msgs;
        std::mutex _msg_q_lock;

        std::condition_variable _has_msg;

    public:
        One_Way(){
        };
        void send(const std::string &next_msg){
            {
                std::unique_lock<std::mutex> l(_msg_q_lock);
                if(closed){
                    throw smpl::Error("Closed");
                }
                else{
                    _msgs.push_back(next_msg);
                }
            }
            _has_msg.notify_one();
        };
        std::string recv(){
            std::unique_lock<std::mutex> l(_msg_q_lock);
            while(_msgs.empty()){
                _has_msg.wait(l);
                if(closed){
                    throw smpl::Error("Closed");
                }
            }
            const std::string m = _msgs.front();
            _msgs.pop_front();
            return m;
        };
        void close(){
            std::lock_guard<std::mutex> l(_msg_q_lock);
            closed = true;
            _has_msg.notify_all();
        };
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

};


std::mutex connection_queues_lock;
std::map<pthread_t, std::deque<std::shared_ptr<Waiting_Connection>>> connection_queues;

Thread_Listener::Thread_Listener(){
    _self = pthread_self();
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
    std::shared_ptr<Waiting_Connection> foo;
    {
        {
            std::unique_lock<std::mutex> l(connection_queues_lock);
            if(connection_queues[_self].empty()){ //No client currently blocked connecting
                std::shared_ptr<Waiting_Connection> w(new Waiting_Connection());
                connection_queues[_self].push_back(w);

            }
            foo = connection_queues[_self].front();
        }
        {
            //CONDITION VARIABLE SHENANIGANS
            std::unique_lock<std::mutex> l(foo->_m);
            foo->connection.server_receiver = receiver;
            if(foo->connection.client_receiver == nullptr){
                //wait
                foo->_c.wait(l);
                sender = foo->connection.client_receiver;
            }
            else{
                //signal
                sender = foo->connection.client_receiver;
                foo->_c.notify_one();
            }
        }
        {
            std::unique_lock<std::mutex> l(connection_queues_lock);
            connection_queues[_self].pop_front();
        }
    }

    return (new Thread_Channel(sender, receiver));
}

bool Thread_Listener::check(){
    std::lock_guard<std::mutex> l(connection_queues_lock);
    return ( !connection_queues[_self].empty() );
}

Thread_ID::Thread_ID(const pthread_t &peer){
    _peer = peer;
}

smpl::Channel* Thread_ID::connect(){
    std::shared_ptr<One_Way> receiver(new One_Way());
    std::shared_ptr<One_Way> sender;
    std::shared_ptr<Waiting_Connection> foo;
    {
        try{
            std::unique_lock<std::mutex> l(connection_queues_lock);
            if(connection_queues.at(_peer).empty() || connection_queues.at(_peer).front()->connection.server_receiver == nullptr){ //server not blocked or we're not next in line
                std::shared_ptr<Waiting_Connection> w(new Waiting_Connection());
                connection_queues.at(_peer).push_back(w);
            }
            else{ //server is blocked listening and we're next
                foo = connection_queues.at(_peer).front();
            }
        }
        catch(std::out_of_range o){
            throw smpl::Error("No listening thread");
        }
        {
            //CONDITION VARIABLE SHENANIGANS
            std::unique_lock<std::mutex> l(foo->_m);
            foo->connection.client_receiver = receiver;
            if(foo->connection.server_receiver == nullptr){
                //wait
                foo->_c.wait(l);
                sender = foo->connection.server_receiver;
            }
            else{
                //signal
                sender = foo->connection.server_receiver;
                foo->_c.notify_one();
            }
        }
    }

    return (new Thread_Channel(sender, receiver));
}

Thread_Channel::Thread_Channel(std::shared_ptr<One_Way> sender, std::shared_ptr<One_Way> receiver){
    _sender = sender;
    _receiver = receiver;
}

Thread_Channel::~Thread_Channel(){
    _sender->close();
    _receiver->close();
}

void Thread_Channel::send(const std::string &msg){
    _sender->send(msg);
}

std::string Thread_Channel::recv(){
    return _receiver->recv();
}
