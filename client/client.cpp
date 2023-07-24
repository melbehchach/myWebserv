#include "client.hpp"

client::client() {
    std::cout << "CREATION DONE" << std::endl;
}


void client::clearBody(void) {
    if (_endRecv)
        _requestBody.clear();
    else if (_endSend)
        _responseBody.clear();
}

client::~client() {}