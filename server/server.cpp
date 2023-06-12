#include "server.hpp"

server::server() {
    if (!_getaddrinfo())
        exit(1);

    if (!_socket())
        exit(1);

    if (!_setsocket()) 
        exit(1);

    if (!_bind())
        exit(1);

    freeaddrinfo(result); 

    if(!_listen())
        exit(1);

    _add_descriptor(sockfd); 
    
    pos_read = 0;
    bytToSend = 0;

    while (1) {
        if (_poll() == -1) 
            break ; // poll call fialed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd)
                    _accept();    // Listening descriptor is readable. 
                else
                    _receive(i); // check the receiving of data
            }
            else if (pfds[i].revents & POLLOUT) {
                _msg = respoo.headers_generator(reqobj._status_code);
                bytes_send = send(pfds[i].fd, _msg.c_str(), BUFFSIZE, 0);
                if (bytes_send < 0) {
                    std::cout << strerror(errno) << '\n';
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                }
                close(pfds[i].fd);
                pfds.erase(pfds.begin() + i);
                _msg.erase();
            }
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
    sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd < 0 || (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)) {
        std::cout << strerror(errno);
        exit (1);
    }
    return (sockfd);
}

bool server::_setsocket(void) {
    int setsock = 1;
    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::_bind(void) {
    if ((bind(sockfd, result->ai_addr, result->ai_addrlen)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

bool server::_listen(void) {
    if ((listen(sockfd, SOMAXCONN)) < 0) {
        std::cout << strerror(errno);
        return (false);
    }
    return (true);
}

int server::_poll(void) {
    int total_fds;
    total_fds = 0;
    total_fds = poll(pfds.data(), pfds.size(), -1);
    if (total_fds < 0) {
        std::cout << strerror(errno);
        return (0);
    }
    return (total_fds);
}

int server::_accept(void) {
    clientaddrln = sizeof(client);
    cltfd = accept(sockfd, (struct sockaddr*)&client, &clientaddrln);
    if (cltfd < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "after cheking the errno \n";
        else
            std::cout << strerror(errno);
    }
    else
        _add_descriptor(cltfd); // fill the pfds vector
    return (cltfd);
}

void server::_add_descriptor(int fd) {
    pollfds.fd = fd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);
}

void server::_receive(int index) {
    memset(buffer, 0, BUFFSIZE);
    bytes_rcv = recv(pfds[index].fd, buffer, (BUFFSIZE - 1), 0);
    buffer[bytes_rcv] = '\0';
    if (bytes_rcv < 0) {
        std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
        return ;
    }
    _msg.append(buffer, bytes_rcv);
    if (bytToSend == 0) {
        reqmsg.assign(buffer);
        reqobj.get_request(reqmsg);
        _path = reqobj._uri;
        pos = _msg.find("\r\n\r\n");
        _msg.erase(0, (pos + 4));
        pos = _msg.find("Content-Type:");
        _msg.erase(0, (pos + 27));
        pos += 28;
    }
    bytToSend += bytes_rcv;
    if ((bytToSend - pos) >= reqobj._content_length) {
        std::cout << bytToSend << '\n';
        pos = _msg.find("--");
        if (pos != -1)
            std::cout << "debug\n";
        _msg.erase(pos, 56);
        std::ofstream filo("test.mp4");
        if (filo.is_open()) {
            filo << _msg;
            filo.close();
        }
        _msg.erase();
        bytToSend = 0;
        pfds[index].events = POLLOUT;
    }
}


// void server::filter_request(const char buf[]) {
//     std::stringstream _content;


// }


server::~server() {}




















// GET METHOD
// if (reqobj._method == "GET") {
//     std::cout << "GET method \n";
//     if (pos_read == 0) {
//         respoo.get_content_type(_path);
//         _msg = respoo.headers_generator(reqobj._status_code);
//         _msg += readFile();
//         pos_read++;
//     }
//     if (_msg.size() > 0) {
//         bytToSend = _msg.size() / 10;
//         if (_msg.size() < BUFFSIZE)
//             bytToSend = _msg.size();
//         bytes_send = send(pfds[i].fd, _msg.c_str(), bytToSend, 0);
//         if (bytes_send < 0)
//             std::cout << strerror(errno) << '\n';
//         _msg.erase(0, bytes_send);
//     }
//     else {
//         close(pfds[i].fd);
//         pfds.erase(pfds.begin() + i);
//         _msg.erase();
//         pos_read = 0;
//     }
// }

// std::string	server::readFile(void) { // RETURN THE CONTENT OF A FILE AS A STD::STRING
// 	std::ifstream 	file;
// 	std::string text;
// 	std::ostringstream streambuff;
// 	file.open(_path);
// 	if (file.is_open()) {
// 		streambuff << file.rdbuf();
// 		text = streambuff.str();
// 	}
// 	file.close();
// 	return text;
// }

// int server::get_file_size(void) {
//     std::ifstream _file(_path, std::ios::in | std::ios::binary | std::ios::ate);
//     if (!_file.is_open())
//         std::cout << "error kabiiiiiir \n";
//     std::streampos _fileSize = _file.tellg();
//     int contentLength = static_cast<int>(_fileSize);
//     _file.close();
//     return (contentLength);
// }




