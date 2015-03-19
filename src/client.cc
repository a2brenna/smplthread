#include "channel.h"
#include "unix_domain_socket.h"
#include "network_socket.h"
#include "hgutil/time.h"

#include <memory> //for unique_ptr
#include <iostream>
#include <chrono>

void test_mechanism(const std::unique_ptr<smpl::Remote_Address> &server_address){
        std::string message;
        message.append("Hello");
        for(int i = 0; i < 1; i++){
            message.append("\nHello");
        }

        const auto start_time = std::chrono::high_resolution_clock::now();
        std::unique_ptr<smpl::Channel> c( server_address->connect() );
        for(int i = 0; i < 1; i++){
            c->send(message);
        }
        const auto end_time = std::chrono::high_resolution_clock::now();

        std::cout << "Elapsed " << (end_time - start_time).count() << std::endl;
}

int main(){
    try{
        std::unique_ptr<smpl::Remote_Address> server_a( new Remote_UDS("/tmp/channel_test.sock"));
        test_mechanism(server_a);
        std::unique_ptr<smpl::Remote_Address> server_b( new Remote_Port("127.0.0.1", 6000));
        test_mechanism(server_b);
    }
    catch(...){
        return -1;
    }
}
