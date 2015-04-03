#include "smpl.h"
#include "smplthread.h"
#include "smplsocket.h"

#include <memory> //for unique_ptr
#include <vector>
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

void peer(const std::thread::id parent){
    std::unique_ptr<smpl::Remote_Address> server( new Thread_ID(parent));
    test_mechanism(server);
}

void thread_test(){
    const int TEST_RUNS = 10000;
    const int MSGS_PER_RUN = 10000;
    const std::thread::id _self = std::this_thread::get_id();

    std::unique_ptr<smpl::Local_Address> server_c( new Thread_Listener() );

    std::set<std::unique_ptr<smpl::Remote_Address>> peers;

    for(int j = 0; j < TEST_RUNS; j++){
        for(int i = 0; i < MSGS_PER_RUN; i++)
        {
            int num_clients = 5;
            std::vector<std::thread> threads;
            for(int k = 0; k < num_clients; k++){
                std::function<void()> peer_thread_foo = std::bind(peer, _self);
                threads.push_back(std::thread(peer_thread_foo));

            }

            std::set<std::unique_ptr<smpl::Channel>> peers;
            for(int k = 0; k < num_clients; k++){
                peers.insert(std::unique_ptr<smpl::Channel>( server_c->listen()));
            }

            for(int i = 0; i < 1; i++){
                for(auto &p: peers){
                    p->send("World");
                    std::string response = p->recv();
                }
            }

            for(auto &t: threads){
                t.join();
            }

            std::cout << j << " " << i << std::endl;
        }
    }


}

void socket_test(){
/*
    peers.insert(new Remote_UDS("/tmp/channel_test.sock"));
    peers.insert(new Remote_Port("127.0.0.1", 6000));
*/
}

int main(){
    thread_test();
    return 0;

}
