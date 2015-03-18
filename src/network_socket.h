#ifndef __NETWORK_SOCKET_H__
#define __NETWORK_SOCKET_H__

#include "remote_address.h"
#include "local_address.h"

class Local_Port: public smpl::Local_Address {

    private:
        std::string ip;
        int port = -1;
        int sockfd = -1;

    public:

        Local_Port(const std::string &new_ip, const int &new_port);

        virtual ~Local_Port();
        virtual smpl::Channel* listen();
        virtual bool check();

};

class Remote_Port : public smpl::Remote_Address {

    private:
        std::string ip;
        int port = -1;

    public:

        Remote_Port(const std::string &new_ip, const int &new_port);
        virtual smpl::Channel* connect();

};

#endif
