#include "channel.h"
#include "unix_domain_socket.h"
#include "network_socket.h"
#include "hgutil/time.h"
#include "thread_channel.h"

#include <memory> //for unique_ptr
#include <set>
#include <iostream>
#include <chrono>
#include <thread>

void test_mechanism(const std::unique_ptr<smpl::Remote_Address> &server_address){

        std::unique_ptr<smpl::Channel> c( server_address->connect() );

        for(int i = 0; i < 1; i++){
            c->send("Hello");
            std::string response = c->recv();
        }
}

void peer(const pthread_t parent){
    std::unique_ptr<smpl::Remote_Address> server( new Thread_ID(parent));
    test_mechanism(server);
}

int main(){

    const int TEST_RUNS = 10000;
    const int MSGS_PER_RUN = 10000;
    const pthread_t _self = pthread_self();

    std::unique_ptr<smpl::Local_Address> server_c( new Thread_Listener() );

    std::set<std::unique_ptr<smpl::Remote_Address>> peers;
    //peers.insert(new Remote_UDS("/tmp/channel_test.sock"));
    //peers.insert(new Remote_Port("127.0.0.1", 6000));


    for(int j = 0; j < TEST_RUNS; j++){
        for(int i = 0; i < MSGS_PER_RUN; i++)
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

            std::unique_ptr<smpl::Channel> client_thread_foo( server_c->listen() );
            std::unique_ptr<smpl::Channel> client_thread_gaf( server_c->listen() );
            std::unique_ptr<smpl::Channel> client_thread_bar( server_c->listen() );
            std::unique_ptr<smpl::Channel> client_thread_sal( server_c->listen() );
            std::unique_ptr<smpl::Channel> client_thread_baz( server_c->listen() );

            for(int i = 0; i < 1; i++){
                client_thread_foo->send("World");
                client_thread_foo->recv();

                client_thread_bar->send("World");
                client_thread_bar->recv();

                client_thread_baz->send("World");
                client_thread_baz->recv();

                client_thread_sal->send("World");
                client_thread_sal->recv();

                client_thread_gaf->send("World");
                client_thread_gaf->recv();
            }

            t_foo.join();
            t_bar.join();
            t_baz.join();
            t_sal.join();
            t_gaf.join();
            std::cout << j << " " << i << std::endl;
        }
    }

    return 0;

}
