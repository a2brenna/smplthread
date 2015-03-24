#include "channel.h"
#include "unix_domain_socket.h"
#include "network_socket.h"
#include "hgutil/time.h"
#include "thread_channel.h"

#include <memory> //for unique_ptr
#include <iostream>
#include <chrono>
#include <thread>

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

        const std::string response = c->recv();
        std::cout << response << std::endl;

        std::cout << "Elapsed " << (end_time - start_time).count() << std::endl;
}

void peer(const pthread_t parent){
    std::unique_ptr<smpl::Remote_Address> server( new Thread_ID(parent));
    test_mechanism(server);
}

int main(){

    std::unique_ptr<smpl::Remote_Address> server_a( new Remote_UDS("/tmp/channel_test.sock"));
    std::unique_ptr<smpl::Remote_Address> server_b( new Remote_Port("127.0.0.1", 6000));
    test_mechanism(server_a);
    test_mechanism(server_b);

    std::unique_ptr<smpl::Local_Address> server_c( new Thread_Listener() );

    const pthread_t _self = pthread_self();
    std::function<void()> peer_thread = std::bind(peer, _self);
    auto t = std::thread(peer_thread);

    std::unique_ptr<smpl::Channel> client_thread( server_c->listen() );

    std::cout << client_thread->recv() << std::endl;
    client_thread->send("World");
    t.join();

    return 0;

}
