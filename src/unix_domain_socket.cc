#include "smplsocket.h"

#include <netdb.h> //for struct addrinfo
#include <sys/un.h> //for sockaddr_un
#include <sys/socket.h> //for socket
#include <unistd.h> //for close

#define UNIX_MAX_PATH 108

smpl::Local_UDS::Local_UDS(const std::string &new_path){

    path = new_path;
    if ( path.size() > UNIX_MAX_PATH ){
        throw smpl::Bad_Address();
    }

    {
        struct addrinfo res;
        struct sockaddr_un address;

        const auto m = memset(&address, 0, sizeof(struct sockaddr_un));
        if( m != &address){
            throw smpl::Open_Failed();
        }

        address.sun_family = AF_UNIX;
        strncpy(address.sun_path, path.c_str(), UNIX_MAX_PATH);
        address.sun_path[UNIX_MAX_PATH - 1] = '\0';

        res.ai_addr = (struct sockaddr *)&address;
        res.ai_addrlen = sizeof(struct sockaddr_un);

        sockfd =  socket(AF_UNIX, SOCK_STREAM, 0);
        if( sockfd < 0 ){
            throw smpl::Open_Failed();
        }

        const int yes = 1;
        const auto a = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        const auto b = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int));
        if ((a != 0) || (b != 0)) {
            throw smpl::Open_Failed();
        }

        const int bind_result = bind(sockfd, res.ai_addr, res.ai_addrlen);
        if( bind_result < 0 ){
            throw smpl::Open_Failed();
        }

        const int l = ::listen(sockfd, SOMAXCONN);
        if( l < 0 ){
            throw smpl::Open_Failed();
        }

    }

}

smpl::Local_UDS::~Local_UDS(){
    const int c = close(sockfd);
    if(c != 0){
        throw smpl::Close_Failed();
    }
    const int u = remove(path.c_str());
    if(u < 0){
        throw smpl::Close_Failed();
    }
}

smpl::Channel* smpl::Local_UDS::listen() noexcept{

    const int a = accept(sockfd, nullptr, nullptr);
    if( a < 0 ){
        return nullptr;
    }

    const auto fd = new File_Descriptor(a);

    if( fd == nullptr ){
        return nullptr;
    }

    return fd;
}

bool smpl::Local_UDS::check() noexcept{
    return false;
}

smpl::Remote_UDS::Remote_UDS(const std::string &new_path){
    path = new_path;
    if ( path.size() > UNIX_MAX_PATH ){
        throw smpl::Bad_Address();
    }
}

smpl::Channel* smpl::Remote_UDS::connect() noexcept{
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if( sockfd < 0 ){
        return nullptr;
    }

    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, path.c_str(), UNIX_MAX_PATH);
    const auto len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    const auto c = ::connect(sockfd, (struct sockaddr *)&remote, len);
    if( c != 0 ){
        return nullptr;
    }

    const auto fd = new File_Descriptor(sockfd);
    if( fd == nullptr ){
        return nullptr;
    }

    return fd;
}
