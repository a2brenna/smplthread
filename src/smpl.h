#ifndef __SMPL_H__
#define __SMPL_H__

#include <string>

//TODO: Need to define subclasses for Error..
//TODO: Need to define non-blocking variant of Channel::ready()

namespace smpl {

    class Channel{

        Channel(const Channel&) = delete;

        public:

            virtual ~Channel() {};

            Channel() {};

            virtual void send(const std::string &msg) noexcept = 0;
            virtual std::string recv() noexcept = 0;
            virtual void wait() noexcept = 0;

    };

    class Remote_Address{

        public:
            virtual Channel* connect() noexcept = 0;

    };

    class Local_Address{

        Local_Address(const Local_Address&) = delete;

        public:

            Local_Address() {};
            virtual ~Local_Address() {};
            virtual Channel* listen() noexcept = 0;
            virtual bool check() noexcept = 0;

    };

    class Bad_Address {};

    class Channel_Closed {};

    class Connection_Failed {};

    class Transport_Failed {};

    class Open_Failed {};

    class Close_Failed {};

}

#endif
