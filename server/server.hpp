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
        std::vector<int> listners;
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
        // std::string                 _port;
        // std::string                 _host;

        // bool                        _startRecv;
        char                        _buffer[16000];
        int                         _socketFd;
        int                         _clinetFd;
        int                         _bytesRecv;
        int                         _totalFds;
        int                         _totalFdsCheck;
        int                         _newClientFd;

    // SERVER METHODS
        bool                        serverGetaddrinfo(std::string &_port, std::string &_host);
        int                         serverSocket(void);
        bool                        serverSetsocket(void);
        bool                        serverBind(void);
        bool                        serverListen(void);
        int                         serverAccept(int fd);
        int                         serverPoll(void);
        void                        addFDescriptor(int fd);
        void                        serverReceive(int fd, int index);
        void                        serverSend(int fd, int index);

    public:

        // 
        std::multimap<int, client>                  _clientsMap;
        std::multimap<int, client>::iterator        _mapIt;
         
        server(std::multimap<std::string, std::pair<std::string, std::string> >const & m);
        ~server();
};

#endif