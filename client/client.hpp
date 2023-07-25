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
        std::string _requestBody; // for request and response body
        std::string _responseBody; // for request and response body
        std::string _port;
        std::string _host;
        std::string _hostname;
        std::string _URI;
        // std::string _method;
        int         _fd;
        bool        _startRecv;
        bool        _startSend;
        // bool        _endRecv;
        // bool        _endSend;
        // int         _clientFd;
        // int         _bytesCounter;  // for the response when serving a file 
        // int         _bytesSend; // for the response when serving a file
        // bool        _stopSend;

        void setFd(int fd);
        void enableStartRecv(void);
        void disableStartRecv(void);
        void enableStartSend(void);
        void disableStartSend(void);
        void clearBody(void);


    client();
    ~client();
};

#endif