#include "client.hpp"

client::client() {
    // Set of attributs
}


void client::setFd(int fd) {
    _fd = fd;
}

void client::enableStartRecv(void) {
    _startRecv = true;
    // std::cout << "strat receive enable" << std::endl;
}

void client::disableStartRecv(void) {
    _startRecv = false;
    // std::cout << "strat receive desabled" << std::endl;
}

void client::enableStartSend(void) {
    _startSend = true;
    // std::cout << "strat send enable" << std::endl;
}

void client::disableStartSend(void) {
    // std::cout << "strat send desabled" << std::endl;
    _startSend = false;
}

// void client::setServer(t_socketListner _server) {

// }

client::~client() {}