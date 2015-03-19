#include "unix_domain_socket.h"
#include "file_descriptor.h"
#include "error.h"

#include <netdb.h> //for struct addrinfo
#include <sys/un.h> //for sockaddr_un
#include <sys/socket.h> //for socket
#include <unistd.h> //for close

#define UNIX_MAX_PATH 108

Local_UDS::Local_UDS(const std::string &new_path){

    path = new_path;
    if ( path.size() > UNIX_MAX_PATH ){
        throw smpl::Error("Path too long");
    }

    {
        struct addrinfo res;
        struct sockaddr_un address;

        const auto m = memset(&address, 0, sizeof(struct sockaddr_un));
        if( m != &address){
            throw smpl::Error("Failed to zero address structure");
        }

        address.sun_family = AF_UNIX;
        strncpy(address.sun_path, path.c_str(), UNIX_MAX_PATH);
        address.sun_path[UNIX_MAX_PATH - 1] = '\0';

        res.ai_addr = (struct sockaddr *)&address;
        res.ai_addrlen = sizeof(struct sockaddr_un);

        sockfd =  socket(AF_UNIX, SOCK_STREAM, 0);
        if( sockfd < 0 ){
            throw smpl::Error("Failed to open socket");
        }

        const int yes = 1;
        const auto a = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        const auto b = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int));
        if ((a != 0) || (b != 0)) {
            throw smpl::Error("Failed to set socket option(s) SO_REUSEADDR or SO_REUSEPORT");
        }

        const int bind_result = bind(sockfd, res.ai_addr, res.ai_addrlen);
        if( bind_result < 0 ){
            throw smpl::Error("Failed to bind socket");
        }

        const int l = ::listen(sockfd, SOMAXCONN);
        if( l < 0 ){
            throw smpl::Error("Failed to listen on socket");
        }

    }

}

Local_UDS::~Local_UDS(){
    const int c = close(sockfd);
    if(c != 0){
        throw smpl::Error("Failed to cleanly close socket");
    }
    const int u = remove(path.c_str());
    if(u < 0){
        throw smpl::Error("Failed to remove unix domain file");
    }
}

smpl::Channel* Local_UDS::listen(){

    const int a = accept(sockfd, nullptr, nullptr);
    if( a < 0 ){
        throw smpl::Error("Failed to get incoming connection");
    }

    const auto fd = new File_Descriptor(a);

    if( fd == NULL ){
        throw smpl::Error("Failed to create File_Descriptor");
    }

    return fd;
}

bool Local_UDS::check(){

    return false;
}

Remote_UDS::Remote_UDS(const std::string &new_path){
    path = new_path;
    if ( path.size() > UNIX_MAX_PATH ){
        throw smpl::Error("Path too long");
    }
}

smpl::Channel* Remote_UDS::connect(){
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if( sockfd < 0 ){
        throw smpl::Error("Could not open socket");
    }

    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, path.c_str(), UNIX_MAX_PATH);
    const auto len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    const auto c = ::connect(sockfd, (struct sockaddr *)&remote, len);
    if( c != 0 ){
        throw smpl::Error("Error connecting to remote port");
    }

    const auto fd = new File_Descriptor(sockfd);
    if( fd == NULL ){
        throw smpl::Error("Failed to create File_Descriptor");
    }

    return fd;
}
