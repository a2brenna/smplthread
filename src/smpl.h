#ifndef __SMPL_H__
#define __SMPL_H__

#include <string>

//TODO: Need to define subclasses for Error..
//TODO: Need to define non-blocking variant of Channel::ready()

namespace smpl {

    class Channel{

        Channel(const Channel&) = delete;

        public:

            virtual ~Channel() noexcept {};

            Channel() {};

            virtual ssize_t send(const std::string &msg) noexcept = 0;
            virtual std::string recv() noexcept = 0;
            virtual bool wait() noexcept = 0;

    };

    class Remote_Address{

        public:
            virtual Channel* connect() noexcept = 0;

    };

    class Local_Address{

        Local_Address(const Local_Address&) = delete;

        public:

            Local_Address() {};
            virtual ~Local_Address() noexcept {};
            virtual Channel* listen() noexcept = 0;
            virtual bool check() noexcept = 0;

    };

    class Bad_Address {};

    class Open_Failed {};

}

#endif
