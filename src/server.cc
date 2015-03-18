#include "channel.h"
#include "unix_domain_socket.h"

#include <memory> //for std::unique_ptr
#include <iostream> //for std::cout

int main(){


    std::unique_ptr<smpl::Local_Address> server(new Local_UDS("/tmp/channel_test.sock"));

    for(;;){

        std::unique_ptr<smpl::Channel> client(server->listen());
        std::string message = client->recv();
        std::cout << message << std::endl;

    }

    return 0;

}
