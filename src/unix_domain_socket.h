#ifndef __UNIX_DOMAIN_SOCKET_H__
#define __UNIX_DOMAIN_SOCKET_H__

#include "remote_address.h"
#include "local_address.h"

class Local_UDS : public smpl::Local_Address {

    private:
        std::string path;
        int sockfd = -1;

    public:

        Local_UDS(const std::string &new_path);

        virtual ~Local_UDS();
        virtual smpl::Channel* listen();
        virtual bool check();

};

class Remote_UDS : public smpl::Remote_Address {

    private:
        std::string path;

    public:

        Remote_UDS(const std::string &new_path);
        virtual smpl::Channel* connect();

};

#endif
