#include "channel.h"
#include "unix_domain_socket.h"

#include <memory>

int main(){
    std::unique_ptr<smpl::Remote_Address> server_address( new Remote_UDS("/tmp/channel_test.sock"));
    std::unique_ptr<smpl::Channel> server( server_address->connect() );
    const std::string message = "Hello";
    server->send(message);
    return 0;
}

