#include "thread_channel.h"

#include <map>
#include <deque>
#include <mutex>

std::mutex connection_queues_lock;
std::map<pthread_t, std::deque<pthread_t>> connection_queues;

Thread_Listener::Thread_Listener(){

}

Thread_Listener::~Thread_Listener(){

}

smpl::Channel* Thread_Listener::listen(){
    return nullptr;
}

bool Thread_Listener::check(){
    return false;
}

Thread_ID::Thread_ID(const pthread_t &peer){

}

smpl::Channel* Thread_ID::connect(){
    return nullptr;
}

Thread_Channel::Thread_Channel(){

}

Thread_Channel::~Thread_Channel(){

}

void Thread_Channel::send(const std::string &msg){

}

std::string Thread_Channel::recv(){
    return "";
}
