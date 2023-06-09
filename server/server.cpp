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
    pos_read = 0;
    bytToSend = 0;
    while (1) {
        nbrfds = _poll();
        if (nbrfds < 0) 
            break ; // poll call fialed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd)
                    _accept();    // Listening descriptor is readable. 
                else {
                    _receive(i); // check the receiving of data
                    if (reqobj._method == "POST") {
                        if (bytToSend < reqobj._content_length) {
                            std::cout << "kan7sb bytosend == " << bytToSend << '\n';
                            bytToSend += BUFFSIZE;
                            continue;
                        }
                    }
                }
            }
            else if (pfds[i].revents & POLLOUT) {
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
                //         retsend = send(pfds[i].fd, _msg.c_str(), bytToSend, 0);
                //         if (retsend < 0)
                //             std::cout << strerror(errno) << '\n';
                //         _msg.erase(0, retsend);
                //     }
                //     else {
                //         close(pfds[i].fd);
                //         pfds.erase(pfds.begin() + i);
                //         _msg.erase();
                //         pos_read = 0;
                //     }
                // }
                // else if (reqobj._method == "POST") {
                    // std::cout << "POST method \n"; 
                    // std::cout << _test;
                    bytToSend = BUFFSIZE;
                    _msg = respoo.headers_generator(reqobj._status_code);
                    // std::cout << _msg << '\n';
                    retsend = send(pfds[i].fd, _msg.c_str(), bytToSend, 0);
                    if (retsend < 0)
                        std::cout << strerror(errno) << '\n';
                    // std::cout << _msg;
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                    _msg.erase();
                // }
            }
        }
    }
}

std::string	server::readFile(void) { // RETURN THE CONTENT OF A FILE AS A STD::STRING
	std::ifstream 	file;
	std::string text;
	std::ostringstream streambuff;
	file.open(_path);
	if (file.is_open()) {
		streambuff << file.rdbuf();
		text = streambuff.str();
	}
	file.close();
	return text;
}

int server::get_file_size(void) {
    std::ifstream _file(_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!_file.is_open())
        std::cout << "error kabiiiiiir \n";
    std::streampos _fileSize = _file.tellg();
    int contentLength = static_cast<int>(_fileSize);
    _file.close();
    return (contentLength);
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
    // _test += buffer;
    if (byt_rcv <= 0) {
        // recv failed
        std::cout << "problem a hamid\n";    
        if (byt_rcv < 0)
            std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
    }
    else {
        // recv acces
        // buffer[byt_rcv] = '\0';
        if (bytToSend == 0) {
            std::cout << "Well received hna\n";
            reqmsg.assign(buffer);
            reqobj.get_request(reqmsg);
            _path = reqobj._uri;
            // _path += "exemple.html"
        }
        _msg += buffer;
        // std::cout << buffer;
        if (bytToSend >= reqobj._content_length) {
            std::ofstream filo("image.html");
            if (filo.is_open()) {
                filo << _msg;
                filo.close();
            }
            std::cout << "Well received lhih\n";
            std::cout << "byt to send " << bytToSend << '\n';
            std::cout << "insid req " <<reqobj._content_length << '\n';
            std::cout << "debug\n";
            _msg.erase();
            pfds[index].events = POLLOUT;
        }
    }
}

void server::_add_fd(int fd) {
    pollfds.fd = fd;
    pollfds.events = POLLIN;
    pfds.push_back(pollfds);
}

server::~server() {}