#ifndef __THREAD_CHANNEL_H__
#define __THREAD_CHANNEL_H__

#include "smpl.h"

#include <deque>
#include <condition_variable>
#include <memory>
#include <thread>

class Thread_Listener : public smpl::Local_Address {

    private:
        std::thread::id _self;

    public:

        Thread_Listener();

        virtual ~Thread_Listener();
        virtual smpl::Channel* listen() noexcept;
        virtual bool check() noexcept;

};

class Thread_ID : public smpl::Remote_Address {

    private:
        std::thread::id _peer;

    public:

        Thread_ID(const std::thread::id &peer);
        virtual smpl::Channel* connect() noexcept;

};

class One_Way;

class Thread_Channel: public smpl::Channel {

    private:
        std::shared_ptr<One_Way> _sender;
        std::shared_ptr<One_Way> _receiver;

    public:

        Thread_Channel(std::shared_ptr<One_Way> _sender, std::shared_ptr<One_Way> _receiver);
        virtual ~Thread_Channel();

        virtual void send(const std::string &msg) noexcept;
        virtual std::string recv() noexcept;
        virtual void wait() noexcept;

};

#endif
