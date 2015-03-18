#include "file_descriptor.h"
#include "error.h"

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

}

std::string File_Descriptor::recv(){

    return "";
}
