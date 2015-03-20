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

};

class Waiting_Connection{

    public:
        std::mutex _m;
        std::condition_variable _c;

        Duplex connection;

};


std::mutex connection_queues_lock;
std::map<pthread_t, std::deque<pthread_t>> connection_queues;

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
    std::lock_guard<std::mutex> l(connection_queues_lock);
    const pthread_t peer = connection_queues[_self].front();
    connection_queues[_self].pop_front();
    return nullptr;
}

bool Thread_Listener::check(){
    std::lock_guard<std::mutex> l(connection_queues_lock);
    return ( !connection_queues[_self].empty() );
}

Thread_ID::Thread_ID(const pthread_t &peer){
    _peer = peer;
}

smpl::Channel* Thread_ID::connect(){
    return nullptr;
}

Thread_Channel::Thread_Channel(std::shared_ptr<One_Way> _sender, std::shared_ptr<One_Way> _receiver){

}

Thread_Channel::~Thread_Channel(){

}

void Thread_Channel::send(const std::string &msg){

}

std::string Thread_Channel::recv(){
    return "";
}
