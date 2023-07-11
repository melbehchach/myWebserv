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
        std::vector<std::string>    _chunksVector;
        std::vector<std::string>    _chunkSizeVector;
        std::ofstream               file;
        std::string                 _body;
        std::string                 _path;
        std::string                 _headers;
        std::string                 _tmpBody;
        std::string                 _chunks;
        char                        buffer[16000];
        int                         socketFd;
        int                         bytesSend;
        int                         clinetFd;
        int                         bytesRecv;
        int                         bytesCounter;
        int                         position1;
        int                         position2;


    // SERVER METHODS
        bool                        _getaddrinfo(void);
        int                         _socket(void);
        bool                        _setsocket(void);
        bool                        _bind(void);
        bool                        _listen(void);
        int                         _accept(void);
        int                         _poll(void);
        void                        _receive(int index);
        void                        _add_descriptor(int fd);

    
        // int     get_file_size(void);
        // std::string	readFile(void);

    public:
        server();
        ~server();
};

#endif