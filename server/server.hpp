#ifndef SERVER_HPP
#define SERVER_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#include "../response/response.hpp"
#include "../client/client.hpp"



#define PORT "9430"
#define BUFFSIZE 16000

class server {
    private:
    // SERVER ATTRIBUTS
        std::vector<struct pollfd>  pfds;
        struct addrinfo             hints;
        struct addrinfo             *result;
        struct sockaddr_in          _client;
        struct pollfd               pollfds;
        socklen_t                   _clientaddrln;
        request                     _request;
        response                    _response;
        std::string                 _path;
        std::string                 _tmpBody;
        bool                        _startrecv;
        char                        _buffer[16000];
        int                         _socketFd;
        int                         _clinetFd;
        int                         _bytesRecv;
        int                         _totalFds;
        int                         _totalFdsCheck;

    // SERVER METHODS
        bool                        serverGetaddrinfo(void);
        int                         serverSocket(void);
        bool                        serverSetsocket(void);
        bool                        serverBind(void);
        bool                        serverListen(void);
        int                         serverAccept(void);
        int                         serverPoll(void);
        void                        addFDescriptor(int fd);
        void                        serverReceive(int index);
        void                        serverSend(int index);

    public:

         
        server();
        ~server();
};

#endif