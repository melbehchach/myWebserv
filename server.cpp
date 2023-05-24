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

    retval = listen(sockfd, 5);
    if (retval < 0) {
        std::cout << strerror(errno) << '\n';
        exit (1);
    }

    pfds.resize(1);
    
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;
    fdcount = 1;


    while (1) 
    {
        // std::cout << "pool wait \n";
        nbrfds = poll(pfds.data(), pfds.size(), -1);
        // std::cout << "pool work \n";
        if (nbrfds < 0) 
        {
            std::cout << strerror(errno) << '\n';
            exit (1);
        }


        size_t xyz = pfds.size();

        for (size_t i = 0; i < xyz || i < pfds.size(); i++) 
        {
            // std::cout << "nloop \n";
            // std::cout << pfds.size() << '\n';
            if (pfds[i].revents & POLLIN) 
            {
                if (pfds[i].fd == sockfd) 
                {
                    clientaddrln = sizeof(client);
                    new_fd = accept(sockfd, (struct sockaddr *)&client, &clientaddrln);
                    if (new_fd < 0) 
                    {
                        std::cout << strerror(errno) << '\n';
                        exit (1);    
                    }
                    else 
                    {
                        pollfds.fd = new_fd;
                        pollfds.events = POLLIN;
                        pfds.push_back(pollfds);
                        fdcount++;
                    }
                }
                else 
                {
                    byt_rcv = recv(pfds[i].fd, buffer, sizeof(buffer), 0);
                    // send(pfds[i].fd, "HTTP/3 204 No Content\r\n\r\n", 100, 0);
                    if (byt_rcv <= 0) 
                    {
                        if (byt_rcv < 0)
                            std::cout << strerror(errno) << '\n';
                        else if (byt_rcv == 0)
                        {
                            std::cout << "cnx closed ... \n";
                            close(pfds[i].fd);
                            pfds.erase(pfds.begin() + i);
                        }
                        // exit (1);    
                    }
                    else 
                    {
                        buffer[byt_rcv] = '\0';
                        std::cout << buffer << '\n';
                    }
                }
            }
            // else if (pfds[i].revents | POLLOUT) 
            // {
            //     std::cout << "time to write.. \n";
            // }
            // else 
            // {
                // close(pfds[i].fd);
                // pfds.erase(pfds.begin() + i);
                // assert(1 == 0);
            // }
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