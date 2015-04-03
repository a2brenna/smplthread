#ifndef __SMPLSOCKET_H___
#define __SMPLSOCKET_H___

#include "smpl.h"

//TODO:Investigate how POSIX fds are reused and see if we can make guarantees
//about whether or not the underlying fd has been changed here... maybe some
//sort of global locked registry?

class File_Descriptor : public smpl::Channel {

    private:
        int _fd;

    public:

        virtual ~File_Descriptor();
        File_Descriptor(const int &fd);

        virtual void send(const std::string &msg);
        virtual std::string recv();

};

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
