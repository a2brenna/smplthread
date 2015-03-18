#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <string>

namespace smpl {

    class Channel{

        Channel(const Channel&) = delete;

        public:

            //virtual ~Channel() = 0;

            Channel() {};

            virtual void send(const std::string &msg) = 0;
            virtual std::string recv() = 0;

    };

}

#endif
