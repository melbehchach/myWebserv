#include "server.hpp"

server::server() {
    if (!_getaddrinfo())
        exit(1);
    if (!_socket())
        exit(1);
    if (!_setsocket()) 
        exit(1);
    if (!_bind()) // in case of error must close the listner
        exit(1);
    freeaddrinfo(result); 
    if(!_listen())
        exit(1);
    _add_descriptor(socketFd);
    while (1) {
        totalFdsCheck = _poll();
        if (totalFdsCheck < 0)
            break ; // poll call fialed
        if (pfds[0].revents & POLLIN) {
            std::cout << "accepting new connection" <<  std::endl;
            _accept();   // Listening descriptor is readable. 
            _startrecv = true;
        }
        for (size_t i = 1; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN)
                _receive(i); // check the receiving of data
            else if (pfds[i].revents & POLLOUT)
                _send(i);
        }
    }
}

bool server::_getaddrinfo(void) {
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

int server::_socket(void) {
    socketFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (socketFd < 0) {
        std::cout << strerror(errno);
        exit (1);
    }
    // if ((fcntl(socketFd, F_SETFL, O_NONBLOCK) < 0)) //  to check the non blocking after tests
    //     std::cout << strerror(errno);
    return (socketFd);
}

bool server::_setsocket(void) {
    int setsock = 1;
    if ((setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::_bind(void) {
    if ((bind(socketFd, result->ai_addr, result->ai_addrlen)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::_listen(void) {
    if ((listen(socketFd, SOMAXCONN)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

int server::_poll(void) {
    totalFds = poll(pfds.data(), pfds.size(), -1);
    if (totalFds < 0) {
        std::cout << strerror(errno);
    }
    return (totalFds);
}

int server::_accept(void) {
    clientaddrln = sizeof(client);
    clinetFd = accept(socketFd, (struct sockaddr*)&client, &clientaddrln);
    if (clinetFd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "after cheking the errno \n";
        else
            std::cout << strerror(errno);
    }
    else 
        _add_descriptor(clinetFd); // fill the pfds vector
    std::cout << clinetFd << std::endl;
    return (clinetFd);
}

void server::_add_descriptor(int fd) {
    pollfds.fd = fd;
    if (fd == socketFd)
        pollfds.events = POLLIN;
    else {
        pollfds.events = POLLIN | POLLOUT;
        pollfds.revents = 0;
    }
    pfds.push_back(pollfds);
}

void server::_receive(int index) {
    memset(buffer, 0, BUFFSIZE);
    bytesRecv = recv(pfds[index].fd, buffer, (BUFFSIZE - 1), 0);
    if (bytesRecv < 0) {
        std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
        exit(0);
    }
    if (pfds[index].revents & POLLOUT)
        std::cout << "ya lghder bda" << std::endl;
    _tmpBody.append(buffer, bytesRecv);
    if (_startrecv) { // stor headers in a multi-map and erase them from the body
        _request.requestHeader(_tmpBody);
        if (_request._method == "POST")
            _request.erasePostRequestHeaders(_tmpBody);
        _path = _request._uri;
        _startrecv = false;
    }
    if (_request._method == "POST") {
        _request.postMethod(_tmpBody);
    }
}

void server::_send(int index) {
    int ret = -1;
    _response.code = _request._status_code;
    if (_request._method == "POST") {
        _response.postMethodResponse(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
    }
    else if (_request._method == "GET") {
        _response._path = _path;
        ret = _response.getMethodResponse(pfds[index].fd);
        if (ret == 0) {
            if (_request._connexion != "keep-alive\r") {
                close(pfds[index].fd);
                pfds.erase(pfds.begin() + index);
            }
            pfds[index].revents = POLLIN;
        }
    }
    _tmpBody.clear();
}

server::~server() {}