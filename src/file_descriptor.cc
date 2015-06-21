#include "smplsocket.h"

#include <arpa/inet.h> //for htonl
#include <unistd.h>

#define READ_WINDOW 4096

smpl::File_Descriptor::~File_Descriptor(){
    close(_fd);
}

smpl::File_Descriptor::File_Descriptor(const int &fd){
    _fd = fd;
}

ssize_t smpl::File_Descriptor::send(const std::string &msg) noexcept{
    std::unique_lock<std::mutex> lock(_write_lock);
    ssize_t msg_length = msg.length();
    uint32_t net_length = htonl(msg_length);


    const auto l = ::send(_fd, &net_length, 4, MSG_NOSIGNAL);
    if ( l < 0 ){
        return -1;
    }

    const auto s = ::send(_fd, msg.c_str(), msg_length, MSG_NOSIGNAL);
    if ( s != msg_length ){
        return -1;
    }
    return s;
}

std::string smpl::File_Descriptor::recv() noexcept{
    std::unique_lock<std::mutex> lock(_read_lock);

    uint32_t net_length;

    if ( ::recv(_fd, &net_length, 4, MSG_NOSIGNAL) != 4){
        return "";
    }

    uint32_t bytes_remaining = ntohl(net_length);

    std::string msg;

    while (msg.length() < bytes_remaining) {
        //PERHAPS REIMPLEMENT USING std::array<char>?
        char buff[READ_WINDOW];
        size_t to_read = std::min((unsigned long)READ_WINDOW, bytes_remaining - msg.length());

        const int ret = ::recv(_fd, buff, to_read, MSG_NOSIGNAL);
        if (ret < 0) {
            return "";
        }
        else {
            msg.append(buff, ret);
        }
    }


    return msg;
}

bool smpl::File_Descriptor::wait() noexcept{
    std::unique_lock<std::mutex> lock(_read_lock);
    fd_set set;
    FD_SET(_fd, &set);

    const int ret = select(_fd + 1, &set, nullptr, nullptr, nullptr);

    if(ret < 0){
        return false;
    }
    else{
        return true;
    }
}
