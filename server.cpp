#include "server.hpp"

server::server() 
{

    struct addrinfo hints;
    struct addrinfo *sock_res;
    // struct addrinfo *sock_list;

    int sock_fd;
    int new_fd;
    int status;
    int setsock_val = 1;


    memset(&hints, 0, sizeof(hints));
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    
    while (1)
    {
            if ((status = getaddrinfo("", PORT, &hints, &sock_res)) < 0)
        {
            std::cout << "getaddd " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }

        if ((sock_fd = socket(sock_res->ai_family, sock_res->ai_socktype, sock_res->ai_protocol)) < 0)
        {
            std::cout << "socket " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }

        if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0)
        {
            std::cout << " fcntl " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }

        if ((status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&(setsock_val), sizeof(setsock_val))) < 0)
        {
            std::cout << "setckopt " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }

        if (bind(sock_fd, sock_res->ai_addr, sock_res->ai_addrlen) < 0)
        {
            std::cout << "bind " << '\n';
            std::cout << strerror(errno) << '\n';

            exit (1);
        }

        if (listen(sock_fd, 1) < 0)
        {
            std::cout << "listen " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }


        freeaddrinfo(sock_res);

        fds.resize(1);
        fds[0].fd = sock_fd;
        fds[0].events = POLLIN;
        // std::cout << fds[0].fd << '\n';

        if (poll(&fds[0], 1, -1) < 0)
        {
            std::cout << "poll " << '\n';
            std::cout << strerror(errno) << '\n';
            exit (1);
        }

        if (fds[0].revents & POLLIN)
        {
            int r;
            char str[1024];
            memset(str, 0, sizeof(str));
            if (fds[0].fd == sock_fd)
            {
                if ((new_fd = accept(sock_fd, sock_res->ai_addr, &sock_res->ai_addrlen)) < 0)
                {
                    std::cout << strerror(errno) << '\n';
                    exit (1);
                }

                fds.resize(2);
                fds[1].fd = new_fd;
                fds[1].events = POLLIN;
                std::cout << "connection made \n";
                r = recv(new_fd, str, sizeof(str), 0);
                if (r < 0){
                    std::cerr << strerror(errno) << std::endl;
                }
                std::cout << str << "\n";
                break ;
            }
        }    
    }
    

}

server::~server() {}