#ifndef __UNIX_DOMAIN_SOCKET_H__
#define __UNIX_DOMAIN_SOCKET_H__

#include "remote_address.h"
#include "local_address.h"

class Local_UDS : smpl::Local_Address {

    public:

        virtual ~Local_UDS();
        virtual smpl::Channel* listen();
        virtual bool check();

};

class Remote_UDS : smpl::Remote_Address {

    public:

        virtual smpl::Channel* connect();

};

#endif
