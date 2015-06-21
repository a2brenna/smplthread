#ifndef __SMPL_H__
#define __SMPL_H__

#include <string>

namespace smpl {

    class Channel{

        Channel(const Channel&) = delete;

        private:
            virtual ssize_t _send(const std::string &msg) noexcept = 0;

        public:

            virtual ~Channel() noexcept {};

            Channel() {};

            ssize_t send(const std::string &msg) noexcept {
                if(msg.empty()){
                    return -1;
                }
                else
                    return _send(msg);
            }
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

}

#endif
