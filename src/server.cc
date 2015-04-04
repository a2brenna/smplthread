#include "smpl.h"
#include "smplsocket.h"

#include <memory> //for std::unique_ptr
#include <iostream> //for std::cout

void test_mechanism(const std::unique_ptr<smpl::Local_Address> &listen_point){
    std::unique_ptr<smpl::Channel> client(listen_point->listen());
    std::string message = client->recv();
    std::cout << message << std::endl;

    client->send("World");
}

int main(){

    for(;;){
        //std::unique_ptr<smpl::Local_Address> server_a(new Local_UDS("/tmp/channel_test.sock"));
        std::unique_ptr<smpl::Local_Address> server_b(new Local_Port("127.0.0.1", 6000));
        //test_mechanism(server_a);
        test_mechanism(server_b);
    }

    return 0;

}
