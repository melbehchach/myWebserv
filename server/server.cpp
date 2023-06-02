#include "server.hpp"

server::server() 
{
    _getaddrinfo(); // fill the struct addrinfo 
    _socket(); // create a socket to listen 
    _setsocket(); // set the address to be reuse
    _bind(); // bind the socket to a specific port (name)
    freeaddrinfo(result); // free the struct
    _listen(); // listen through the socket
    _add_fd(sockfd); // add the lestining socket to the pfds vector

    while (1) {
        nbrfds = _poll();
        if (nbrfds < 0) 
            break ; // poll call fialed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd)
                    _accept();    // Listening descriptor is readable. 
                else
                    _receive(i); // check the receiving of data
            }
            if (pfds[i].revents & POLLOUT) {
                respoo.response_generator(reqobj._status_code);
                _msg = respoo._status_line;
                _msg += "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                        "Server: Apache/2.2.14 (Win32)\r\n"
                        "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                        "Content-Length: 88\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: Closed\r\n"
                        "\r\n";
                byt_rcv = send(pfds[i].fd, _msg.c_str(), _msg.size(), 0);
                if (byt_rcv < 0)
                    std::cout << strerror(errno) << '\n';
                close(pfds[i].fd);
                pfds.erase(pfds.begin() + i);
            }
        }
    }
}

void server::_getaddrinfo(void) {

    // void for the moment after that i must pass the result of the parser <multimap>
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    retval = getaddrinfo("localhost", PORT, &hints, &result);
    if (retval < 0)
    {
        std::cout <<  gai_strerror(errno) << '\n';
        exit (1);
    }
}

int server::_socket(void) {

    sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd < 0 || (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (sockfd);
}

int server::_setsocket(void) {

    int setsock = 1;
    retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock));
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::_bind(void) {

    retval = bind(sockfd, result->ai_addr, result->ai_addrlen);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::_listen(void) {
    
    retval = listen(sockfd, SOMAXCONN);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::_poll(void) {

    retval = poll(pfds.data(), pfds.size(), -1);
    if (retval < 0)
        std::cout << strerror(errno) << '\n';
    return (retval);
}

int server::_accept(void) {

    clientaddrln = sizeof(client);
    cltfd = accept(sockfd, (struct sockaddr*)&client, &clientaddrln);
    if (cltfd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "after cheking the errno \n";
        else
            std::cout << strerror(errno) << '\n';
    }
    else
        _add_fd(cltfd); // fill the pfds vector
    return (cltfd);
}

void server::_receive(int index) {
    
    byt_rcv = recv(pfds[index].fd, buffer, sizeof(buffer), 0);
    if (byt_rcv <= 0) {
        // recv failed
        if (byt_rcv < 0)
            std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
    }
    else {
        // recv acces
        buffer[byt_rcv] = '\0';
        reqmsg.assign(buffer);
        reqobj.get_request(reqmsg);
        pfds[index].events = POLLOUT;
    }
}

void server::_add_fd(int fd) {
    pollfds.fd = fd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);
}

server::~server() {}



// "HTTP/1.1 404 not found\r\n"
//                                             "Content-Type: text/plain\r\n"
//                                             "Content-Length: 13\r\n"
//                                             "\r\n"
//                                             "Hello, World!"