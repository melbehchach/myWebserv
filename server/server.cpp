#include "server.hpp"

bool server::serverGetaddrinfo(std::string &_port, std::string &_host) {
    // void for the moment after that i must pass the result of the parser <multimap>
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result)) < 0) {
        std::cerr <<  gai_strerror(errno);
        return (false);
    }
    return (true);
}

int server::serverSocket(void) {
    _socketFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_socketFd < 0) {
        std::cerr << strerror(errno);
        exit (1);
    }
    // listners.push_back(_socketFd);
    return (_socketFd);
}

bool server::serverSetsocket(void) {
    int setsock = 1;
    if ((setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
        std::cerr << strerror(errno);
        return (false);
    }
    fcntl(_socketFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
    return (true);
}

bool server::serverBind(void) {
    if ((bind(_socketFd, result->ai_addr, result->ai_addrlen)) < 0) {
        std::cerr << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::serverListen(void) {
    if ((listen(_socketFd, SOMAXCONN)) < 0) {
        std::cerr << strerror(errno);
        return (false);
    }
    return (true);
}

int server::serverPoll(void) {
    _totalFds = poll(&pfds[0], pfds.size(), -1);
    if (_totalFds < 0) {
        std::cerr << strerror(errno);
    }
    return (_totalFds);
}

int server::serverAccept(int fd) {
    _clientaddrln = sizeof(_client);
    _clinetFd = accept(fd, (struct sockaddr*)&_client, &_clientaddrln);
    fcntl(_clinetFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
    if (_clinetFd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cerr << strerror(errno) << " error \n";
        else {

            std::cerr << strerror(errno) << "hamid \n";
        }
    }
    else 
        addFDescriptor(_clinetFd); // fill the pfds vector
    return (_clinetFd);
}

void server::addFDescriptor(int fd) {
    pollfds.fd = fd;
    if (pollfds.fd == _socketFd)
        pollfds.events = POLLIN | POLLHUP;
    else
        pollfds.events = POLLIN | POLLOUT | POLLHUP;
    pfds.push_back(pollfds);
}

server::server(std::multimap<std::string, std::pair<std::string, std::string> >const & m, ConfigFileParser const & _File) :_configFile(_File)  {
    
    // std::cout <<  _configFile.GetServers()[0].GetRoot() << std::endl;
    // std::cout <<  "uri of location:=> " << _configFile.GetServers()[0].GetLocationContexts()[0].GetLocationUri().GetUri() << std::endl;

    // std::string tmp;

    // tmp = _configFile.GetServers()[0].GetRoot();
    // tmp.append(_configFile.GetServers()[0].GetLocationContexts()[0].GetLocationUri().GetUri());
    // std::cout << _configFile.GetServers()[0].GetServerNames()[0] << std::endl;

    // std::cout << tmp << std::endl;

    // for (size_t i = 0; i < _configFile.GetServers()[0].GetLocationContexts().size(); i++) {
    //     std::cout << _configFile.GetServers()[0].GetLocationContexts()[i].GetLocationUri().GetUri() << std::endl;
    // }

    for (std::multimap<std::string, std::pair<std::string, std::string> >::const_iterator it = m.begin() ; it != m.end(); it++) {
    
        t_socketListner sock;

        sock._host = it->second.first;
        sock._port = it->first;
        sock._serverName = it->second.second;
        
        if (!serverGetaddrinfo(sock._port, sock._host))
            exit(1);
        if (!serverSocket())
            exit(1);
        if (!serverSetsocket()) 
            exit(1);
        if (!serverBind()) // in case of error must close the listner
            exit(1);
        freeaddrinfo(result); 
        if(!serverListen())
            exit(1);
        addFDescriptor(_socketFd);

        _listnersMap.insert( std::pair<int, t_socketListner>(_socketFd, sock) );
    }
    
    for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++) {
    }

    while (1) {
        _totalFdsCheck = serverPoll();
        if (_totalFdsCheck < 0)
            break ; // poll call failed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (i < _listnersMap.size()) {
                    for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++) { // serch for the sokcet that contains event
                        
                        if (pfds[i].fd == _listnersIt->first) { // Creation of a new client
                            client _clientObj; 
                            _newClientFd = serverAccept(pfds[i].fd);   // Listening descriptor is readable.
                            _clientObj.setFd(_newClientFd);
                            _clientObj.enableStartRecv();
                            _clientObj._port = _listnersIt->second._port;
                            _clientObj._host = _listnersIt->second._host;
                            _clientObj._hostname = _listnersIt->second._serverName;
                            _clientsMap.insert( std::pair<int, client>(_newClientFd, _clientObj) );
                            break;
                        }

                    }
                }
                else {
                    serverReceive(pfds[i].fd, i); // check event for receive
                }
            }
            else if (pfds[i].revents & POLLOUT) { // check event for send
                serverSend(pfds[i].fd, i);
            }
            else if (pfds[i].revents & POLLHUP) {
                std::cerr << "CLIENT GOT PROBLEMS" << std::endl;
            }
        }
    }
}

void server::serverReceive(int fd, int index) {
    _mapIt = _clientsMap.find(fd);
    
    if (_mapIt != _clientsMap.end()) {
        
        memset(_buffer, 0, BUFFSIZE);
        _bytesRecv = recv(_mapIt->second._fd, _buffer, BUFFSIZE, 0);
        if (_bytesRecv < 0) {
            close(fd);
            pfds.erase(pfds.begin() + index);  
            _clientsMap.erase(_mapIt);
            return ;
        }
        
        _mapIt->second._requestBody.append(_buffer, _bytesRecv);

        if (_mapIt->second._startRecv) { // stor headers in a multi-map and erase them from the body
            _request.requestHeader(_mapIt->second._requestBody);
            _response.code = 200;

            if(!LocationAvilability(_mapIt->second)) {   // IS THE LOCATION AVAILABLE IN THE SERVER
                std::cout << "location not found" << std::endl;
                _response.code = 404;
            }
            else if (!RedirectionAvilability()) { // DOES IT HAVE A REDIRECTION
                std::cout << "redirecti zmer" << std::endl;
                _response.code = 301;
            }
            else if (!AllowedMthods()) { // WHAT METHODES AR ALLOWED
                std::cout << "ma method ma pikala" << std::endl;
                _response.code = 405;
            }
            else {
                _mapIt->second.disableStartRecv();
                if (_request._method == "POST")
                    _request.erasePostRequestHeaders(_mapIt->second);
            }

        }

        // Handle the request details

        if (_request._method == "POST") {
            _request.postMethod(_mapIt->second);
        }
        else if (_request._method == "GET") { // WHAT HAPPENS IF THE METHOD IS GET
            std::cout << "response code: " << _response.code << std::endl;
            if (_response.code == 200) {
                _mapIt->second._requestBody.clear();
                _URI = AppendRootAndUri();
                if (_URI.size() == 0) {
                    std::cout << "it's not the addressr requested: " << _URI << std::endl;
                    _response.code = 404;
                }
                else {
                    // std::cout << "to response" << std::endl;
                    _response._path = _URI; // MUST UPDATE THE RESPONSE PATH
                }
            }
            _mapIt->second.enableStartSend();
        }
        else if (_request._method == "DELETE") {
            std::remove(_request._URI.c_str());
            _mapIt->second._requestBody.clear();
            _mapIt->second.enableStartSend();
        }

    }
}

void server::serverSend(int fd, int index) {
    _mapIt = _clientsMap.find(fd);
    if (_mapIt != _clientsMap.end()) {

        if (_request._method == "GET") {
            if (_response.getMethodResponse(_mapIt->second)) {
                if (_request._connection != "keep-alive\r") {
                    close(_mapIt->second._fd);
                    pfds.erase(pfds.begin() + index);  
                    _clientsMap.erase(_mapIt);
                }
                _mapIt->second.enableStartRecv();
                _request._method.clear();
            }
        }
        else if ((_request._method == "POST") && _mapIt->second._startSend) {
            _response.postMethodResponse(_mapIt->second);
            if (_request._connection != "keep-alive\r") {
                close(_mapIt->second._fd);
                pfds.erase(pfds.begin() + index);
                _clientsMap.erase(_mapIt);
            }
            _mapIt->second.enableStartRecv();
        }
        else if (_request._method == "DELETE") {
            _response.deleteMethodResponse(_mapIt->second);
            if (_request._connection != "keep-alive\r") {
                close(_mapIt->second._fd);
                pfds.erase(pfds.begin() + index);
                _clientsMap.erase(_mapIt);
            }
            _mapIt->second.enableStartRecv();
        }

    }
}


bool server::LocationAvilability(client & _client) {
    _serverIndex = -1;
    _locationIndex = -1;

    std::cout << "client number: " << _client._fd << std::endl;
    for (size_t i = 0; i < _configFile.GetServers().size(); i++) { // GET THE SERVER
        for (size_t j = 0; j < _configFile.GetServers()[i].GetPortNumbers()[j].size(); j++) {
            if (_client._port == _configFile.GetServers()[i].GetPortNumbers()[j]){
                _serverIndex = i;
                std::cout << "found it: " << _serverIndex << std::endl;
                break;
            }
        }
        if (_serverIndex != -1)
            break; // TO BREAK THE SECOND LOOP
    }

    std::cout << "server number: " << _serverIndex << std::endl;
    std::cout << "Location: " << _request._URI << std::endl;
    
    for (size_t i = 0; i < _configFile.GetServers()[_serverIndex].GetLocationContexts().size(); i++) { // LOOKING FOR THE LOCATION INSIDE THE SERVER
        if (_request._URI == _configFile.GetServers()[_serverIndex].GetLocationContexts()[i].GetLocationUri().GetUri()) {
            _locationIndex = i;
            return (true);
        }
    }
    return (false);
}


bool server::RedirectionAvilability(void) {
    if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetReturn().GetCode() != 301) {
        return (true);
    }
    return (false);
}


bool server::AllowedMthods(void) {
    for (size_t i = 0; i < _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetMethods()[i].size(); i++) {
        if (_request._method == _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetMethods()[i]) {
            return (true);
        }
    }
    return (false);
}

std::string server::AppendRootAndUri(void) {
    std::string tmpURI;
    std::string tmp;
    DIR *directory;

    tmpURI = (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot());
    std::cout << "root path: " << tmpURI << std::endl;
    
    tmpURI.append(_request._URI);
    std::cout << "URI: " << tmpURI << std::endl;
    
    // NOW CHECK IF I HAVE AN INDEX FILE TO SERVE
    // IF NOT CHECK AUTOINDEX IS ON TO LIST CONTENT OF LOCATION DIRECTORY
    // IF AUTOINDEX IS OFF RESPONSE BY 403 FORBIDEN
    // IF NOTHING FOUND 404 NOT FOUND

    std::cout << "number of files inside the location: " << _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex().size() << std::endl;
    std::cout << "number of location: " << _locationIndex << std::endl;
    
    for (size_t i = 0; i < _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex().size(); i++)
    {
        std::cout << _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[i] << std::endl;
    }    

    
    if ((directory = opendir(tmpURI.c_str())) != nullptr) {
        // tmp = tmpURI.substr((tmpURI.size() - 1), 1);
        // if (tmp != "/") {
        //     std::cout << "must redirect the folder" << std::endl;
        //     _response.code = 301;
        // }
    }

    // CHECK THE RESOURCE REQUESTED IF IT IS A FILE OR DIRECTORY

    if (access(tmpURI.c_str(), F_OK | R_OK | W_OK ) == -1) {
        std::cout << "error access path" << std::endl;
        tmpURI.clear();
    }


    return (tmpURI);
}




server::~server() {}