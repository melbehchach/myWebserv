#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../header.hpp"

class client {

    private:

    public:
        std::string _body;
        int         _bytesCounter;
        int         _bytesSend;
    client();
    ~client();
};

#endif