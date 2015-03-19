#ifndef __LOCAL_ADDRESS_H__
#define __LOCAL_ADDRESS_H__

namespace smpl {

    class Local_Address{

        Local_Address(const Local_Address&) = delete;

        public:

            Local_Address() {};
            virtual ~Local_Address() {};
            virtual Channel* listen() = 0;
            virtual bool check() = 0;

    };

}

#endif
