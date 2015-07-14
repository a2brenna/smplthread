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
        virtual smpl::Channel* _listen() noexcept;

    public:

        Thread_Listener();

        virtual ~Thread_Listener();
        virtual smpl::ADDRESS_STATUS check() noexcept;

};

class Thread_ID : public smpl::Remote_Address {

    private:
        std::thread::id _peer;
        virtual smpl::Channel* _connect() noexcept;

    public:

        Thread_ID(const std::thread::id &peer);

};

class One_Way;

class Thread_Channel: public smpl::Channel {

    private:
        std::shared_ptr<One_Way> _sender;
        std::shared_ptr<One_Way> _receiver;
        virtual ssize_t _send(const std::string &msg) noexcept;
        virtual ssize_t _recv(std::string &msg) noexcept;

    public:

        Thread_Channel(std::shared_ptr<One_Way> _sender, std::shared_ptr<One_Way> _receiver);
        virtual ~Thread_Channel();
        virtual smpl::CHANNEL_STATUS wait() noexcept;

};

#endif
