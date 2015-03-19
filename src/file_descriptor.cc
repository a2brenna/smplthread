#include "file_descriptor.h"
#include "error.h"

#include <arpa/inet.h> //for htonl
#include <unistd.h>

#define READ_WINDOW 4096

File_Descriptor::~File_Descriptor(){
    const int c = close(_fd);
    if (c != 0){
        throw smpl::Error("Failed to close socket");
    }
}

File_Descriptor::File_Descriptor(const int &fd){
    _fd = fd;
}

void File_Descriptor::send(const std::string &msg){
    ssize_t msg_length = msg.length();
    uint32_t net_length = htonl(msg_length);

    const auto l = ::send(_fd, &net_length, 4, MSG_NOSIGNAL);
    if ( l < 0 ){
        throw smpl::Error("Failed to send msg length");
    }

    const auto s = ::send(_fd, msg.c_str(), msg_length, MSG_NOSIGNAL);
    if ( s != msg_length ){
        throw smpl::Error("Failed to send msg");
    }
}

std::string File_Descriptor::recv(){

    uint32_t net_length;

    if ( ::recv(_fd, &net_length, 4, MSG_NOSIGNAL) != 4){
        throw smpl::Error("Failed to receive msg length");
    }

    uint32_t bytes_remaining = ntohl(net_length);

    std::string msg;

    while (msg.length() < bytes_remaining) {
        //PERHAPS REIMPLEMENT USING std::array<char>?
        char buff[READ_WINDOW];
        size_t to_read = std::min((unsigned long)READ_WINDOW, bytes_remaining - msg.length());

        const int ret = ::recv(_fd, buff, to_read, MSG_NOSIGNAL);
        if (ret < 0) {
            throw smpl::Error("Recv loop failed");
        }
        else {
            msg.append(buff, ret);
        }
    }


    return msg;
}
