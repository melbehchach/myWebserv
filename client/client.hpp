#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../header.hpp"

class client {
    private:

    public:
        std::string _requestBody; // for request and response body
        std::string _responseBody; // for request and response body
        std::string _port;
        std::string _host;
        std::string _hostname;
        std::string _method;
        std::string _uploadPath;
        int         _fd;
        bool        _startRecv;
        bool        _startSend;
        bool        _endSend;
        bool        _serveFile;
        bool        _autoIndexOn;
        bool        _cgiOn;

        void setFd(int fd);
        void enableStartRecv(void);
        void disableStartRecv(void);
        void enableStartSend(void);
        void disableStartSend(void);
        void resetAttributs(void);

    client();
    ~client();
};

#endif
