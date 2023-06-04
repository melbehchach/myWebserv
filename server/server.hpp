#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#include "../response/response.hpp"


#define PORT "9430"

class request;

class server {
    

    private:
        std::vector<struct pollfd>  pfds;
        struct addrinfo             hints;
        struct addrinfo             *result;
        struct sockaddr_in          client;
        struct pollfd               pollfds;
        socklen_t                   clientaddrln;
        int                         sockfd;
        int                         cltfd;
        int                         retval;
        int                         byt_rcv;
        int                         nbrfds;
        char                        buffer[1024];
        std::string                 reqmsg;
        request                     reqobj;
        response                    respoo;
        std::string                 _msg;
        std::string                 _path;

        void    _getaddrinfo(void);
        int     _socket(void);
        int     _setsocket(void);
        int     _bind(void);
        int     _listen(void);
        int     _accept(void);
        int     _poll(void);
        void    _receive(int index);
        void    _add_fd(int fd);

    public:
        server();
        ~server();
};

#endif