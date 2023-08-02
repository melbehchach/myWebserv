#ifndef SERVER_HPP
#define SERVER_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#include "../response/response.hpp"
#include "../client/client.hpp"
#include "../parsing/headers/ConfigFileParser.hpp"



#define BUFFSIZE 16000



typedef struct socketListner {
    std::string _port;
    std::string _host;
    std::string _serverName;
} t_socketListner;

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
        char                        _buffer[16000];
        int                         _socketFd;
        int                         _clinetFd;
        int                         _bytesRecv;
        int                         _totalFds;
        int                         _totalFdsCheck;
        int                         _newClientFd;
        int                         _serverIndex;
        int                         _locationIndex;
        ConfigFileParser const &    _configFile;
        std::string                 _URI;
        bool                        _isFile;
        bool                        _isDirectory;

    
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
        bool                        LocationAvilability(client & _client);
        bool                        RedirectionAvilability(void);
        bool                        AllowedMthods(void);
        std::string                 AppendRootAndUri(void);
        // FOR GET METHOD
        void                        serveDirecotry(client &_client);
        void                        serveFile(void);
        // void                        createHtmlFile(std::string fName, std::string list);
        // void                        destroyHtmlFile(void);


    

    public:

        // 
        std::multimap<int, client>                  _clientsMap;
        std::multimap<int, client>::iterator        _mapIt;
        std::multimap<int, socketListner>                  _listnersMap;
        std::multimap<int, socketListner>::iterator        _listnersIt;
         
        server(std::multimap<std::string, std::pair<std::string, std::string> >const & m, ConfigFileParser const &);
        ~server();
};

#endif