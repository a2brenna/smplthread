#ifndef __SMPLSOCKET_H___
#define __SMPLSOCKET_H___

#include "smpl.h"
#include <mutex>

//TODO:Investigate how POSIX fds are reused and see if we can make guarantees
//about whether or not the underlying fd has been changed here... maybe some
//sort of global locked registry?
//

namespace smpl{

class File_Descriptor : public smpl::Channel {

    private:
        int _fd;
        std::mutex _read_lock;
        std::mutex _write_lock;

    public:

        virtual ~File_Descriptor();
        File_Descriptor(const int &fd);

        virtual ssize_t send(const std::string &msg) noexcept;
        virtual std::string recv() noexcept;
        virtual bool wait() noexcept;

};

class Local_Port: public smpl::Local_Address {

    private:
        std::string ip;
        int port = -1;
        int sockfd = -1;
        bool _initialize(const std::string &new_ip, const int &new_port) noexcept;

    public:

        Local_Port(const std::string &new_ip, const int &new_port){
            if(_initialize(new_ip, new_port)){
                return;
            }
            else{
                throw Bad_Address();
            }
        };

        virtual ~Local_Port();
        virtual smpl::Channel* listen() noexcept;
        virtual bool check() noexcept;

};

class Remote_Port : public smpl::Remote_Address {

    private:
        std::string ip;
        int port = -1;
        bool _initialize(const std::string &new_ip, const int &new_port) noexcept;

    public:

        Remote_Port(const std::string &new_ip, const int &new_port){
            if(_initialize(new_ip, new_port)){
                return;
            }
            else{
                throw Bad_Address();
            }
        };
        virtual smpl::Channel* connect() noexcept;

};

class Local_UDS : public smpl::Local_Address {

    private:
        std::string path;
        int sockfd = -1;
        bool _initialize(const std::string &new_path) noexcept;

    public:

        Local_UDS(const std::string &new_path){
            if(_initialize(new_path)){
                return;
            }
            else{
                throw Bad_Address();
            }
        };

        virtual ~Local_UDS();
        virtual smpl::Channel* listen() noexcept;
        virtual bool check() noexcept;

};

class Remote_UDS : public smpl::Remote_Address {

    private:
        std::string path;
        bool _initialize(const std::string &new_path) noexcept;

    public:

        Remote_UDS(const std::string &new_path){
            if(_initialize(new_path)){
                return;
            }
            else{
                throw Bad_Address();
            }
        };
        virtual smpl::Channel* connect() noexcept;

};

}

#endif
