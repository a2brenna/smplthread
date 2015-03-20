#include "thread_channel.h"

#include <map>
#include <deque>
#include <mutex>

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
