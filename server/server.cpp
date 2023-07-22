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
    if (fd == _socketFd)
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
            break ; // poll call fialed
        if (pfds[0].revents & POLLIN) { // pfds[0].fd == _socketFd && 
            serverAccept();   // Listening descriptor is readable.
            std::cout << "nwely accepted: " << _clinetFd <<  std::endl;
            _startrecv = true;
        }
        for (size_t i = 1; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                // std::cout << "read detected at: " << pfds[i].fd << std::endl;
                serverReceive(i); // check the receiving of data
            }
            else if (pfds[i].revents & POLLOUT)
                serverSend(i);
            else if (pfds[i].revents & POLLHUP)
                std::cout << "error in the fd" << std::endl;
        }
    }
}

void server::serverReceive(int index) {
    std::cout << "new request: " << pfds[index].fd << std::endl;
    memset(_buffer, 0, BUFFSIZE);
    _bytesRecv = recv(pfds[index].fd, _buffer, BUFFSIZE, 0);
    if (_bytesRecv < 0) {
        std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
        exit(0);
    }
    _tmpBody.append(_buffer, _bytesRecv);
    // std::cout << _tmpBody << std::endl;
    // std::cout << _tmpBody.size() << std::endl;
    if (_startrecv) { // stor headers in a multi-map and erase them from the body
        _request.requestHeader(_tmpBody);
        _path = _request._URI;
        _response._path = _path;
        _response.code = _request._statusCode;
        _startrecv = false;
        if (_request._method == "POST")
            _request.erasePostRequestHeaders(_tmpBody);
        else{
            std::cout << "to the response" << std::endl;
            _response._startSend = true;
            _tmpBody.clear();
            // pfds[index].events = POLLOUT;
        }
    }
    if (_request._method == "POST") {
        _request.postMethod(_tmpBody);
        if (_tmpBody.size() == 0) {
            std::cout << "response" << std::endl;
            pfds[index].events = POLLOUT; // FORCE THE POLLOUT EVENT
        }
    }
}

void server::serverSend(int index) {
    if (_request._method == "POST") {
        _response.postMethodResponse(pfds[index].fd);
        if (_request._connexion != "keep-alive\r") {
            close(pfds[index].fd);
            pfds.erase(pfds.begin() + index);
        }
    }
    else if (_request._method == "GET") {
        // _it = _clients.find(pfds[index].fd);
        // if (_it != _clients.end()) {
            // std::cout << "debug" << std::endl;
            std::cout << "int of client FD: " << pfds[index].fd << std::endl;
            if (_response.getMethodResponse(pfds[index].fd)) {
                if (_request._connexion != "keep-alive\r") {
                    close(pfds[index].fd);
                    pfds.erase(pfds.begin() + index);
                }
                _startrecv = true; 
                // pfds[index].events = POLLIN; // FORCR THE POLLIN EVENT
            }
        // }
    }
}

server::~server() {}