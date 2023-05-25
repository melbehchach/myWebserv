#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>
#include <exception>
#include <poll.h>
#include <unistd.h> // sysconf(), _SC_OPEN_MAX
#include <cstring>
#define PORT "9430"

class server
{
    private:
        std::vector<struct pollfd>  pfds;
        struct addrinfo             hints;
        struct addrinfo             *result;
        struct sockaddr_in          client;
        struct pollfd               pollfds;
        // struct pollfd               *array;
        socklen_t                   clientaddrln;
        int                         sockfd;
        int                         new_fd;
        int                         retval;
        int                         byt_rcv;
        // int                         nonblock;
        int                         setsock;
        // int                         newfd;
        // int                         fdcount;
        int                         nbrfds;
        char                        buffer[1024];

        // void    ft_getaddrinfo(void);
        // int     ft_socket(void);
        // int     ft_setsocket(void);
        // int     ft_bind(void);
        // int     ft_listen(void);
        // int     ft_accept(void);
        // int     ft_poll(void);

    public:
        server();
        ~server();
};

#endif