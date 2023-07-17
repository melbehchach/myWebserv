#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#include "../response/response.hpp"


#define PORT "9430"
#define BUFFSIZE 16000


class request;

class server {
    

    private:
    // SERVER ATTRIBUTS
        std::vector<struct pollfd>  pfds;
        struct addrinfo             hints;
        struct addrinfo             *result;
        struct sockaddr_in          client;
        struct pollfd               pollfds;
        socklen_t                   clientaddrln;
        request                     _request;
        response                    _response;
        std::string                 _path;
        std::string                 _tmpBody;
        bool                        _startrecv;
        char                        buffer[16000];
        int                         socketFd;
        int                         clinetFd;
        int                         bytesRecv;
        int                         totalFds;
        int                         totalFdsCheck;

    // SERVER METHODS
        bool                        _getaddrinfo(void);
        int                         _socket(void);
        bool                        _setsocket(void);
        bool                        _bind(void);
        bool                        _listen(void);
        int                         _accept(void);
        int                         _poll(void);
        void                        _add_descriptor(int fd);
        void                        _receive(int index);
        void                        _send(int index);

    public:
        server();
        ~server();
};

#endif