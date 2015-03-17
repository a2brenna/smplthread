#ifndef __LOCAL_ADDRESS_H__
#define __LOCAL_ADDRESS_H__

namespace smpl {

    class Local_Address{

        Local_Address(const Local_Address&) = delete;

        public:

            virtual ~Local_Address() = 0;
            virtual Channel* listen() = 0;
            virtual bool check() = 0;

    };

}

#endif
