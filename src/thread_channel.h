#ifndef __THREAD_CHANNEL_H__
#define __THREAD_CHANNEL_H__

#include "remote_address.h"
#include "local_address.h"
#include "channel.h"

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

class Thread_Channel: public smpl::Channel {

    private:

    public:

        Thread_Channel();
        virtual ~Thread_Channel();

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

#endif
