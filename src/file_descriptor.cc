#include "file_descriptor.h"
#include "error.h"

#include <arpa/inet.h> //for htonl
#include <unistd.h>


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

    if ( ::send(_fd, &net_length, 4, MSG_NOSIGNAL) != 4 ){
        throw smpl::Error("Failed to send msg length");
    }

    if ( ::send(_fd, msg.c_str(), msg_length, MSG_NOSIGNAL) != msg_length ){
        throw smpl::Error("Failed to send msg");
    }
}

std::string File_Descriptor::recv(){

    return "";
}
