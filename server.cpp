#include "server.hpp"

server::server() 
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    retval = getaddrinfo("localhost", PORT, &hints, &result);
    if (retval < 0) {
        std::cout <<  gai_strerror(errno) << '\n';
        exit (1);
    }


    sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd < 0)
    {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    retval = fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if (retval < 0)
    {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    setsock = 1;

    retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock));
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    retval = bind(sockfd, result->ai_addr, result->ai_addrlen);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    freeaddrinfo(result);

    retval = listen(sockfd, SOMAXCONN);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    pollfds.fd = sockfd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);

    new_fd = 1;

    while (1) 
    {
        nbrfds = poll(pfds.data(), pfds.size(), -1);
        if (nbrfds < 0) 
        {
            // POLL CALL FAILED
            std::cout << strerror(errno) << '\n';
            break ;
        }
        // One or more descriptors are readable.  Need to determine which ones they are.
        // int current_size = pfds.size();

        for (size_t i = 0; i < pfds.size(); i++) 
        {
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == sockfd) 
                {
                    // Listening descriptor is readable. 
                    clientaddrln = sizeof(client);
                    new_fd = accept(sockfd, (struct sockaddr*)&client, &clientaddrln);
                    if (new_fd < 0) 
                    {
                            // accept failed break the loop
                        if (errno == EWOULDBLOCK || errno == EAGAIN)
                            std::cout << "after cheking the errno \n";
                        else
                            std::cout << strerror(errno) << '\n';
                    }
                    pollfds.fd = new_fd;
                    pollfds.events = POLLIN;
                    pfds.push_back(pollfds);
                    // break;
                }
                else
                {
                    byt_rcv = recv(pfds[i].fd, buffer, sizeof(buffer), 0);
                    if (byt_rcv <= 0)
                    {
                        // recv fialed
                        if (byt_rcv < 0)
                            std::cout << strerror(errno) << '\n';
                        close(pfds[i].fd);
                        pfds.erase(pfds.begin() + i);
                        // break;
                    }
                    else
                    {
                        // recv acces
                        buffer[byt_rcv] = '\0';
                        std::cout << buffer << '\n';
                        pfds[i].events = POLLOUT;
                        // break;
                    }
                }
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

// int server::ft_getaddrinfo(void) {

//     // void for the moment after that i must pass the result of the parser <multimap>
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;
//     hints.ai_flags = AI_PASSIVE;
//     hints.ai_socktype = SOCK_STREAM;

//     retval = getaddrinfo("localhost", PORT, &hints, &result);
//     if (retval < 0)
//     {
//         std::cout <<  gai_strerror(errno) << '\n';
//         exit (1);
//     }
// }

// int server::ft_socket(void) {

//     sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//     if (sockfd < 0 || (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0))
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (sockfd);
// }

// int server::ft_setsocket(void) {

//     setsock = 1;
//     retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock));
//     if (retval < 0)
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (retval);
// }

// int server::ft_bind(void) {

//     retval = bind(sockfd, result->ai_addr, result->ai_addrlen);
//     if (retval < 0)
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (retval);
// }

// int server::ft_listen(void) {
    
//     retval = listen(sockfd, 5);
//     if (retval < 0)
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (retval);
// }

// int server::ft_accept(void) {

//     newfd = accept(sockfd, result->ai_addr, result->ai_addrlen);
//     if (newfd < 0)
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (newfd);
// }

// int server::ft_poll(void) {

//     if ((eventfd = poll(pfds, pfds.size(), 3000)) < 0)
//     {
//         std::cout << strerror(errno) << '\n';
//         exit (1);
//     }
//     return (eventfd);
// }


server::~server() {}