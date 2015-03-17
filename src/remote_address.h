#ifndef __REMOTE_ADDRESS_H__
#define __REMOTE_ADDRESS_H__

#include "channel.h"

namespace smpl {

    class Remote_Address{

        public:
            virtual Channel* connect() = 0;

    };

}

#endif
