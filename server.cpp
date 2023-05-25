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

    pollfds.fd = sockfd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);


    new_fd = -2;

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
        std::cout << "hala wala \n";
        // int current_size = pfds.size();

        for (size_t i = 0; i < pfds.size(); i++) 
        {
            // std::cout <<
            // Loop through to find the descriptors that returned POLLIN and determine whether it's the listening or the active connection.                         
            if (pfds[i].revents == 0)
                continue;

            if (pfds[i].revents != POLLIN)
            {
                // If revents is not POLLIN, it's an unexpected result, log and end the server.
                // Still should know if i must end the server
                std::cout << "error in revernts \n";
                // break;
            }
            
            // if (pfds[i].revents & POLLIN)
            // {

                std::cout << "debug 1 \n";
                if (pfds[i].fd == sockfd) 
                {
                    std::cout << "start \n";

                    while (new_fd != -1)
                    {
                        clientaddrln = sizeof(client);
                        new_fd = accept(sockfd, (struct sockaddr *)&client, &clientaddrln);
                        std::cout << "accepting cnx.... \n";
                        std::cout << "new fd == " << new_fd << '\n';;
                        if (new_fd < 0) 
                        {
                            // SHOULD I EXIT OR CONTINUE 
                            std::cout << strerror(errno) << '\n';
                            break;
                            // exit (1);    
                        }

                        std::cout << "size of vector befor :: " << pfds.size() << '\n';

                        pollfds.fd = new_fd;
                        pollfds.events = POLLIN;
                        pfds.push_back(pollfds);

                        std::cout << pfds[0].fd << '\n';
                        std::cout << pfds[1].fd << '\n';


                        std::cout << "size of vector after :: " << pfds.size() << '\n';
                    }
                }
                else 
                {
                    std::cout << "hello from other condition \n";
                    while (true)
                    {
                        byt_rcv = recv(pfds[i].fd, buffer, sizeof(buffer), 0);
                        if (byt_rcv < 0)
                        {
                            // SHOULD I EXIT OR CONTINUE
                            std::cout << strerror(errno) << '\n';
                            close(pfds[i].fd);
                            pfds.erase(pfds.begin() + i);
                            break;
                        }

                        if (byt_rcv == 0)
                        {
                            std::cout << "cnx closed ... \n";
                            close(pfds[i].fd);
                            pfds.erase(pfds.begin() + i);
                            break;
                        }
                        
                        else 
                        {
                            buffer[byt_rcv] = '\0';
                            std::cout << buffer << '\n';
                        }
                        
                        byt_rcv = send(pfds[i].fd, "HTTP/3 204 No Content\r\n\r\n", 100, 0);
                        if (byt_rcv < 0)
                        {
                            std::cout << strerror(errno) << '\n';
                            close(pfds[i].fd);
                            pfds.erase(pfds.begin() + i);
                            break;
                        }
                    }
                }
                std::cout << "debug 2 \n";
            // }
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