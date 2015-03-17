#include "unix_domain_socket.h"

Local_UDS::~Local_UDS(){

}

smpl::Channel* Local_UDS::listen(){

    return NULL;
}

bool Local_UDS::check(){

    return false;
}

smpl::Channel* Remote_UDS::connect(){

    return NULL;
}
