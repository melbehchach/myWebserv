#include "server.hpp"

server::server() 
{
    ft_getaddrinfo(); // fill the struct addrinfo 
    ft_socket(); // create a socket to listen 
    ft_setsocket(); // set the address to be reuse
    ft_bind(); // bind the socket to a specific port (name)
    freeaddrinfo(result); // free the struct
    ft_listen(); // listen through the socket
    ft_add_fd(sockfd); // add the lestining socket to the pfds vector

    while (1) {
        nbrfds = ft_poll();
        if (nbrfds < 0) 
            break ; // poll call fialed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd)
                    ft_accept();    // Listening descriptor is readable. 
                else
                    ft_receive(i); // check the receiving of data
            }
            if (pfds[i].revents & POLLOUT)
            {
                byt_rcv = send(pfds[i].fd, "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 1024\r\n"
                        "\r\n"
                        "Hamid rak nadi it works !", 1024, 0);
                if (byt_rcv < 0)
                    std::cout << strerror(errno) << '\n';
                close(pfds[i].fd);
                pfds.erase(pfds.begin() + i);
            }
        }
    }
}

void server::ft_getaddrinfo(void) {

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

int server::ft_socket(void) {

    sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd < 0 || (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (sockfd);
}

int server::ft_setsocket(void) {

    int setsock = 1;
    retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock));
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::ft_bind(void) {

    retval = bind(sockfd, result->ai_addr, result->ai_addrlen);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::ft_listen(void) {
    
    retval = listen(sockfd, SOMAXCONN);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }
    return (retval);
}

int server::ft_poll(void) {

    retval = poll(pfds.data(), pfds.size(), -1);
    if (retval < 0)
        std::cout << strerror(errno) << '\n';
    return (retval);
}

int server::ft_accept(void) {

    clientaddrln = sizeof(client);
    cltfd = accept(sockfd, (struct sockaddr*)&client, &clientaddrln);
    if (cltfd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "after cheking the errno \n";
        else
            std::cout << strerror(errno) << '\n';
    }
    else
        ft_add_fd(cltfd); // fill the pfds vector
    return (cltfd);
}

void server::ft_receive(int index) {
    
    byt_rcv = recv(pfds[index].fd, buffer, sizeof(buffer), 0);
    if (byt_rcv <= 0) {
        // recv fialed
        if (byt_rcv < 0)
            std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
    }
    else {
        // recv acces
        buffer[byt_rcv] = '\0';
        std::cout << buffer << '\n';
        pfds[index].events = POLLOUT;
    }
}

void server::ft_add_fd(int fd) {

    pollfds.fd = fd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);
}

server::~server() {}