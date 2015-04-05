#include "smplsocket.h"

#include <netdb.h> //for struct addrinfo
#include <sys/un.h> //for sockaddr_un
#include <sys/socket.h> //for socket
#include <unistd.h> //for close
#include <memory> //for unique_ptr
#include "tripwire.h" //for safe use of freeaddrinfo()

#include <sstream>

smpl::Local_Port::Local_Port(const std::string &new_ip, const int &new_port){

    ip = new_ip;
    //check validity of ip
    port = new_port;
    //check range on port

    std::stringstream s;
    s << port;
    const std::string port_string = s.str();

    struct addrinfo *r = nullptr;
    Tripwire t(std::bind(freeaddrinfo, r));

    const int addrinfo_status = getaddrinfo(ip.c_str(), port_string.c_str(), nullptr, &r);
    if (addrinfo_status != 0) {
        throw smpl::Error("Bad addrinfo");
    }
    if ( r == nullptr ){
        throw smpl::Error("Failed to get addrinfo");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw smpl::Error("Failed to open socket");
    }

    const int yes = 1;
    const auto sa = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    const auto sb = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int));
    if ((sa != 0) || (sb != 0)) {
        throw smpl::Error("Failed to set socket options");
    }

    bool bound = false;
    for(auto s = r; s != nullptr; s = s->ai_next){
        const int b = bind(sockfd, s->ai_addr, s->ai_addrlen);
        if (b == 0) {
            bound = true;
            break;
        }
        else{
            continue;
        }
    }
    if( !bound ){
        throw smpl::Error("Failed to bind() on socket");
    }

    const int l = ::listen(sockfd, SOMAXCONN);
    if (l < 0) {
        throw smpl::Error("Failed to listen() on socket");
    }
}

smpl::Local_Port::~Local_Port(){
    const int c = close(sockfd);
    if(c != 0){
        throw smpl::Error("Failed to cleanly close socket");
    }
}

smpl::Channel* smpl::Local_Port::listen(){

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

bool smpl::Local_Port::check(){

    return false;
}

smpl::Remote_Port::Remote_Port(const std::string &new_ip, const int &new_port){
    ip = new_ip;
    //check validity of ip
    port = new_port;
    //check range on port
}

smpl::Channel* smpl::Remote_Port::connect(){
    std::stringstream s;
    s << port;
    const std::string port_string = s.str();

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *r = nullptr;
    Tripwire t(std::bind(freeaddrinfo, r));

    const auto addrinfo_status = getaddrinfo(ip.c_str(), port_string.c_str(), &hints, &r);
    if (addrinfo_status != 0) {
        throw smpl::Error("Failed to get addrinfo");
    }
    if ( r == nullptr ){
        throw smpl::Error("Failed to get addrinfo");
    }

    int sockfd = -1;
    for(auto s = r; s != nullptr; s = s->ai_next){
        int _s;
        _s = socket(AF_INET, SOCK_STREAM, 0);
        if (_s < 0) {
            throw smpl::Error("Failed to open socket");
        }

        const int c = ::connect(_s , s->ai_addr, s->ai_addrlen);
        if (c < 0) {
            close(_s);
        }
        else{
            sockfd = _s;
            break;
        }
    }
    if(sockfd < 0){
        throw smpl::Error("Failed to connect");
    }

    return new smpl::File_Descriptor(sockfd);
}
