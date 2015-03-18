#include "channel.h"
#include "unix_domain_socket.h"

#include <memory> //for unique_ptr
#include <iostream>
#include <chrono>

int main(){
    std::string message;
    for(int i = 0; i < 10000000; i++){
        message.append("Hello");
    }

    const auto start_time = std::chrono::high_resolution_clock::now();
    std::unique_ptr<smpl::Remote_Address> server_address( new Remote_UDS("/tmp/channel_test.sock"));
    std::unique_ptr<smpl::Channel> server( server_address->connect() );
    server->send(message);
    const auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << "Elapsed " << (end_time - start_time).count() << std::endl;
    return 0;
}

