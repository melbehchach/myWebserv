#include "server.hpp"

bool server::serverGetaddrinfo(void) {
    // void for the moment after that i must pass the result of the parser <multimap>
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((getaddrinfo("localhost", PORT, &hints, &result)) < 0) {
        std::cout <<  gai_strerror(errno);
        return (false);
    }
    return (true);
}

int server::serverSocket(void) {
    _socketFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_socketFd < 0) {
        std::cout << strerror(errno);
        exit (1);
    }
    fcntl(_socketFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
    return (_socketFd);
}

bool server::serverSetsocket(void) {
    int setsock = 1;
    if ((setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::serverBind(void) {
    if ((bind(_socketFd, result->ai_addr, result->ai_addrlen)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::serverListen(void) {
    if ((listen(_socketFd, SOMAXCONN)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

int server::serverPoll(void) {
    _totalFds = poll(&pfds[0], pfds.size(), -1);
    if (_totalFds < 0) {
        std::cout << strerror(errno);
    }
    return (_totalFds);
}

int server::serverAccept(void) {
    _clientaddrln = sizeof(_client);
    _clinetFd = accept(_socketFd, (struct sockaddr*)&_client, &_clientaddrln);
    fcntl(_clinetFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
    if (_clinetFd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "after cheking the errno \n";
        else
            std::cout << strerror(errno);
    }
    else 
        addFDescriptor(_clinetFd); // fill the pfds vector
    return (_clinetFd);
}

void server::addFDescriptor(int fd) {
    pollfds.fd = fd;
    if (pollfds.fd == _socketFd)
        pollfds.events = POLLIN;
    else
        pollfds.events = POLLIN | POLLOUT | POLLHUP;
    pfds.push_back(pollfds);
}

server::server() {
    if (!serverGetaddrinfo())
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
    while (1) {
        _totalFdsCheck = serverPoll();
        if (_totalFdsCheck < 0)
            break ; // poll call failed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == _socketFd && i == 0) { 
                    client _clientObj;
                    _newClientFd = serverAccept();   // Listening descriptor is readable.
                    _clientObj.setFd(_newClientFd);
                    _clientObj.enableStartRecv();
                    _clientsMap.insert( std::pair<int, client>(_newClientFd, _clientObj) );
                    // _clientObj._startRecv = true; // To get HTTP header (request headers)
                    std::cout << "size of map: " << _clientsMap.size() << std::endl;
                    // std::cout << "nwely accepted: " << _clientObj._fd <<  std::endl;
                    // continue;
                }
                else
                    serverReceive(pfds[i].fd); // check the receiving of data
            }
            else if (pfds[i].revents & POLLOUT)
                serverSend(pfds[i].fd);
            else if (pfds[i].revents & POLLHUP)
                std::cerr << "CLIENT GOT PROBLEMS" << std::endl;
        }
    }
}

void server::serverReceive(int fd) {
    _mapIt = _clientsMap.find(fd);
    if (_mapIt != _clientsMap.end()) {
        std::cout << "new request: " << _mapIt->second._fd << std::endl;
        memset(_buffer, 0, BUFFSIZE);
        _bytesRecv = recv(fd, _buffer, BUFFSIZE, 0);
        if (_bytesRecv < 0) {
            std::cout << strerror(errno) << '\n';
            close(fd);
            std::cout << "close" << std::endl;
            // pfds.erase(pfds.begin() + index);  handle how to erase the fd from the poll vectors
            // the request should be empted
            // exit(0);
        }
        _mapIt->second._requestBody.append(_buffer, _bytesRecv);
        if (_mapIt->second._startRecv) { // stor headers in a multi-map and erase them from the body
            std::cout << "request from client: " << _mapIt->second._fd << std::endl;
            std::cout << _mapIt->second._requestBody << std::endl;
            _request.requestHeader(_mapIt->second._requestBody);
            _response._path  = _request._URI;
            _response.code = _request._statusCode;
            _mapIt->second._startRecv = false;
            if (_request._method == "POST")
                _request.erasePostRequestHeaders(_mapIt->second._requestBody);
        }
        if (_request._method == "POST") {
            _request.postMethod(_mapIt->second._requestBody);
        }
        else if (_request._method == "GET") {
            _mapIt->second._requestBody.clear();
            _mapIt->second._responseBody.clear();
            std::cout << "to the response" << std::endl;
            // _response._startSend = true;
        }
    }
}

void server::serverSend(int fd) {
    _mapIt = _clientsMap.find(fd);
    if (_mapIt != _clientsMap.end()) {
        // if (_request._method == "POST") {
        //     _response.postMethodResponse(fd);
        //     if (_request._connexion != "keep-alive\r") {
        //         close(fd);
        //         pfds.erase(pfds.begin() + index);
        //     }
        // }
        if (_request._method == "GET") {
            std::cout << "size of file: " << _mapIt->second._responseBody.size() << std::endl;
            // std::cout << "int of client FD: " << fd << std::endl;
            if (_response.getMethodResponse(_mapIt->second)) {
                std::cout << "end of send" << std::endl;
                if (_request._connexion != "keep-alive\r") {
                    close(_mapIt->second._fd);
                    // pfds.erase(pfds.begin() + index); handle how to erase the fd from the poll vectors
                }
                _mapIt->second._startRecv = false;
                // pfds[index].events = POLLIN; // FORCR THE POLLIN EVENT
            }
        }
    }
}

server::~server() {}