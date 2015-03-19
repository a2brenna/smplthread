#ifndef __THREAD_CHANNEL_H__
#define __THREAD_CHANNEL_H__

#include "remote_address.h"
#include "local_address.h"
#include "channel.h"

#include <mutex>
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

class Uni_Directional_Channel {

    private:
        std::mutex read_lock;
        std::string msg;
        std::mutex write_lock;


    public:
        Uni_Directional_Channel(){
            write_lock.unlock();
            read_lock.lock();
        };
        void send(const std::string &next_msg){
            {
                std::unique_lock<std::mutex> l_a(write_lock);
                msg = next_msg;
                read_lock.unlock();

                //write_lock locked
                //read_lock unlocked

                l_a.release();
                //re-acquire write_lock, forces waiting until write_lock.unlock() is called in reader
                std::unique_lock<std::mutex> l_b(write_lock);
                //read_lock is locked again by here
            }
            //write_lock unlocked
            //read_lock locked
        };
        std::string recv(){
            std::string m;
            {
                std::unique_lock<std::mutex> l(read_lock);
                m = msg;
                l.release(); //read_lock stays locked, waiting for a sender to unlock it
                //write_lock locked
                //read_lock locked
            }
            write_lock.unlock(); //unlock since its now safe to overwrite msg
            //write_lock unlocked
            //read_lock locked
            return m;
        };
};

class Thread_Channel: public smpl::Channel {

    private:

    public:

        Thread_Channel();
        virtual ~Thread_Channel();

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

#endif
