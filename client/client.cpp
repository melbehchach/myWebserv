#include "client.hpp"

client::client() {
    // _startRecv = false;
    // _startSend = false;
    // _endSend = false;
    // _serveFile = false;
    // _autoIndex = false;
    // _isIndexFile = false;
    // _defaultIndexFiles = false;
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

void client::resetAttributs(void) {
    _requestBody.clear();
    _responseBody.clear();
}
client::~client() {}