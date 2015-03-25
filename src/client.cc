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
/*
    std::unique_ptr<smpl::Remote_Address> server_a( new Remote_UDS("/tmp/channel_test.sock"));
    std::unique_ptr<smpl::Remote_Address> server_b( new Remote_Port("127.0.0.1", 6000));
    test_mechanism(server_a);
    test_mechanism(server_b);
    */

    std::unique_ptr<smpl::Local_Address> server_c( new Thread_Listener() );

    //TODO: test with multiple client threads
    const pthread_t _self = pthread_self();
    {
        std::function<void()> peer_thread_foo = std::bind(peer, _self);
        auto t_foo = std::thread(peer_thread_foo);

        std::function<void()> peer_thread_bar = std::bind(peer, _self);
        auto t_bar = std::thread(peer_thread_bar);

        std::function<void()> peer_thread_baz = std::bind(peer, _self);
        auto t_baz = std::thread(peer_thread_baz);

        std::function<void()> peer_thread_sal = std::bind(peer, _self);
        auto t_sal = std::thread(peer_thread_sal);

        std::function<void()> peer_thread_gaf = std::bind(peer, _self);
        auto t_gaf = std::thread(peer_thread_gaf);

        std::unique_ptr<smpl::Channel> client_thread_gaf( server_c->listen() );
        std::unique_ptr<smpl::Channel> client_thread_bar( server_c->listen() );
        std::unique_ptr<smpl::Channel> client_thread_baz( server_c->listen() );
        std::unique_ptr<smpl::Channel> client_thread_sal( server_c->listen() );
        std::unique_ptr<smpl::Channel> client_thread_foo( server_c->listen() );

        std::cout << client_thread_foo->recv() << std::endl;
        client_thread_foo->send("World");
        t_foo.join();

        std::cout << client_thread_bar->recv() << std::endl;
        client_thread_bar->send("World");
        t_bar.join();

        std::cout << client_thread_baz->recv() << std::endl;
        client_thread_baz->send("World");
        t_baz.join();

        std::cout << client_thread_sal->recv() << std::endl;
        client_thread_sal->send("World");
        t_sal.join();

        std::cout << client_thread_gaf->recv() << std::endl;
        client_thread_gaf->send("World");
        t_gaf.join();
    }

    return 0;

}
