#ifndef __THREAD_CHANNEL_H__
#define __THREAD_CHANNEL_H__

#include "remote_address.h"
#include "local_address.h"
#include "channel.h"
#include "error.h"

#include <deque>
#include <condition_variable>
#include <memory>

class Thread_Listener : public smpl::Local_Address {

    private:
        pthread_t _self;

    public:

        Thread_Listener();

        virtual ~Thread_Listener();
        virtual smpl::Channel* listen();
        virtual bool check();

};

class Thread_ID : public smpl::Remote_Address {

    private:
        pthread_t _peer;

    public:

        Thread_ID(const pthread_t &peer);
        virtual smpl::Channel* connect();

};

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

class Thread_Channel: public smpl::Channel {

    private:
        std::shared_ptr<One_Way> _sender;
        std::shared_ptr<One_Way> _receiver;

    public:

        Thread_Channel(std::shared_ptr<One_Way> _sender, std::shared_ptr<One_Way> _receiver);
        virtual ~Thread_Channel();

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

#endif
