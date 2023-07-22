#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../header.hpp"
// #include "../server/server.hpp"
// #include "../request/request.hpp"
// #include "../response/response.hpp"

class client {

    private:

    public:
        std::string _body; // for request and response body
        std::string _port;
        std::string _host;
        std::string _hostname;
        std::string _path;
        int         _clientFd;
        int         _bytesCounter;  // for the response when serving a file 
        int         _bytesSend; // for the response when serving a file
        bool        _stopSend;


    client();
    ~client();
};

#endif