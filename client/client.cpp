#include "client.hpp"

client::client() {
    std::cout << "CREATION DONE" << std::endl;
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

// void client::clearBody(void) {
//     if (_endRecv)
//         _requestBody.clear();
//     else if (_endSend)
//         _responseBody.clear();
// }

client::~client() {}