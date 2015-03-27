#ifndef __SMPL_H__
#define __SMPL_H__

#include <string>

namespace smpl {

    class Channel{

        Channel(const Channel&) = delete;

        public:

            virtual ~Channel() {};

            Channel() {};

            virtual void send(const std::string &msg) = 0;
            virtual std::string recv() = 0;

    };

    class Remote_Address{

        public:
            virtual Channel* connect() = 0;

    };

    class Local_Address{

        Local_Address(const Local_Address&) = delete;

        public:

            Local_Address() {};
            virtual ~Local_Address() {};
            virtual Channel* listen() = 0;
            virtual bool check() = 0;

    };

    class Error {

        private:
            std::string _m;

        public:
            std::string msg() const {
                return _m;
            };
            Error(const std::string &new_msg) {
                _m = new_msg;
            };

    };

}

#endif
