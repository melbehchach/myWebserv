#include "server.hpp"

bool server::serverGetaddrinfo(std::string &_port, std::string &_host) {
    // void for the moment after that i must pass the result of the parser <multimap>
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result)) < 0) {
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
    listners.push_back(_socketFd);
    return (_socketFd);
}

bool server::serverSetsocket(void) {
    int setsock = 1;
    if ((setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    fcntl(_socketFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
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

int server::serverAccept(int fd) {
    _clientaddrln = sizeof(_client);
    _clinetFd = accept(fd, (struct sockaddr*)&_client, &_clientaddrln);
    fcntl(_clinetFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
    if (_clinetFd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << strerror(errno) << " error \n";
        else {

            std::cout << strerror(errno) << "hamid \n";
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

server::server(std::multimap<std::string, std::pair<std::string, std::string> >const & m) {
    for (std::multimap<std::string, std::pair<std::string, std::string> >::const_iterator it = m.begin() ; it != m.end(); it++) {
        std::string host = it->second.first;
        std::string port = it->first;
        std::string serverName = it->second.second;
        if (!serverGetaddrinfo(port, host))
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
        // std::cout << _socketFd << std::endl;
        // std::cout << "size of vector: " << pfds.size() << std::endl;
    }

    while (1) {
        _totalFdsCheck = serverPoll();
        if (_totalFdsCheck < 0)
            break ; // poll call failed
        for (size_t i = 0; i < pfds.size(); i++) {
            // std::cout << "check fd satate: " << pfds[i].fd << std::endl;
            if (pfds[i].revents & POLLIN) {
                if (i < listners.size()) {
                    if (pfds[i].fd == listners[i]) {
                        std::cout << "listner number: " << listners[i] << std::endl;
                        client _clientObj;
                        _newClientFd = serverAccept(pfds[i].fd);   // Listening descriptor is readable.
                        _clientObj.setFd(_newClientFd);
                        _clientObj.enableStartRecv();
                        std::cout << "Accept new client: " << _newClientFd << std::endl;
                        _clientsMap.insert( std::pair<int, client>(_newClientFd, _clientObj) );
                    }
                }
                else
                    serverReceive(pfds[i].fd, i); // check the receiving of data
            }
            else if (pfds[i].revents & POLLOUT) {
                serverSend(pfds[i].fd, i);
            }
            else if (pfds[i].revents & POLLHUP)
                std::cerr << "CLIENT GOT PROBLEMS" << std::endl;
        }
    }
}

void server::serverReceive(int fd, int index) {
    _mapIt = _clientsMap.find(fd);
    if (_mapIt != _clientsMap.end()) {
        // std::cout << "request from client: " << fd << std::endl;
        memset(_buffer, 0, BUFFSIZE);
        _bytesRecv = recv(_mapIt->second._fd, _buffer, BUFFSIZE, 0);
        if (_bytesRecv < 0) {
            // std::cout << "hello error" << std::endl;
            close(fd);
            pfds.erase(pfds.begin() + index);  
            _clientsMap.erase(_mapIt);
            return ;
        }
        _mapIt->second._requestBody.append(_buffer, _bytesRecv);
        // std::cout << _mapIt->second._requestBody << std::endl;
        if (_mapIt->second._startRecv) { // stor headers in a multi-map and erase them from the body
            // std::cout << "first check" << std::endl;
            _request.requestHeader(_mapIt->second._requestBody);
            _response._path  = _request._URI;
            _response.code = _request._statusCode;
            _mapIt->second.disableStartRecv();
            if (_request._method == "POST") 
                _request.erasePostRequestHeaders(_mapIt->second);
        }
        // std::cout << _bytesRecv << std::endl;
        // std::cout << _request._method << std::endl;
        if (_request._method == "POST") {
            // std::cout << "proceed to upload" << std::endl;
            _request.postMethod(_mapIt->second);
        }
        else if (_request._method == "GET") {
            _mapIt->second._requestBody.clear();
            _mapIt->second.enableStartSend();
        }
    }
}

void server::serverSend(int fd, int index) {
    _mapIt = _clientsMap.find(fd);
    if (_mapIt != _clientsMap.end()) {
        if (_request._method == "POST" && _mapIt->second._startSend) {
            _response.postMethodResponse(_mapIt->second);
            if (_request._connection != "keep-alive\r") {
                close(_mapIt->second._fd);
                pfds.erase(pfds.begin() + index);
                _clientsMap.erase(_mapIt);
            }
            _mapIt->second.enableStartRecv();
        }
        if (_request._method == "GET") {
            if (_response.getMethodResponse(_mapIt->second)) {
                // std::cout << "end of send" << std::endl;
                if (_request._connection != "keep-alive\r") {
                    close(_mapIt->second._fd);
                    pfds.erase(pfds.begin() + index);  
                    _clientsMap.erase(_mapIt);
                }
                _mapIt->second.enableStartRecv();
            }
        }
    }
}

server::~server() {}