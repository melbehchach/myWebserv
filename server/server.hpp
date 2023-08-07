#ifndef SERVER_HPP
#define SERVER_HPP

#include "../header.hpp"
#include <sstream>
#include "../request/request.hpp"
#include "../response/response.hpp"
#include "../client/client.hpp"
#include "../parsing/headers/ConfigFileParser.hpp"
#include "../parsing/headers/ServerContext.hpp"
#include "../parsing/headers/LocationContext.hpp"
#include "../cgi/headers/CGI.hpp"


#define BUFFSIZE 16000



typedef struct socketListner {
    std::string _port;
    std::string _host;
    std::string _serverName;
} t_socketListner;

class server {
    private:
    // SERVER ATTRIBUTS

        ConfigFileParser const &    _configFile;
        std::vector<struct pollfd>  pfds;
        struct addrinfo             hints;
        struct addrinfo             *result;
        struct sockaddr_in          _client;
        struct pollfd               pollfds;
        socklen_t                   _clientaddrln;
        request                     _request;
        response                    _response;
        std::string                 _URI;
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
        bool                        _isDirectory;
        bool                        _IndexFiles;
        std::string                 _pathForDelete;
        bool                        _firstResourceCheck;
        std::string                 _Query;


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
        void                        getCurrentServer(client &_client);

        // FIRST CHECK
        bool                        LocationAvilability(void);
        bool                        RedirectionAvilability(void);
        bool                        AllowedMethods(void);
        // SECOND CHECK
        void                        getResourceType(client &_client);
        std::string                 AppendRootAndUri(void);
        void                        UriAvilability(void);
        void                        serveDirecotry(client &_client);


        bool                        IndexExist(void);
        void                        ServeIndexFile(client &_client);


        // DELETE METHOD
        void                        deleteFile(void);
        void                        deleteLocation(void);
        int                         deleteDirectoryContent(std::string const path);
        int                         deleteSubDirectories(std::string const path);


    public:

        //
        std::multimap<int, client>                  _clientsMap;
        std::multimap<int, client>::iterator        _mapIt;
        std::multimap<int, socketListner>                  _listnersMap;
        std::multimap<int, socketListner>::iterator        _listnersIt;

        server(std::multimap<std::string, std::pair<std::string, std::string> >const & m, ConfigFileParser const &);
        ~server();
        void parseCgiOutput(std::string &input, std::ostringstream &header, std::string const &ex);

};

#endif
