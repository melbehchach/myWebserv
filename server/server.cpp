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

    _add_descriptor(sockfd);
    
    // pos_read = 0;
    bytesCounter = 0;
    while (1) {
        if (_poll() == -1)
            break ; // poll call fialed
        for (size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd)
                    _accept();   // Listening descriptor is readable. 
                else
                    _receive(i); // check the receiving of data
            }
            else if (pfds[i].events & POLLOUT) {
                _body = _response.headers_generator(_request._status_code);
                bytesSend = send(pfds[i].fd, _body.c_str(), BUFFSIZE, 0);
                if (bytesSend < 0) {
                    std::cout << strerror(errno) << '\n';
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                }
                // if (_request._connexion != "keep-alive") {
                    close(pfds[i].fd);
                    pfds.erase(pfds.begin() + i);
                    _body.erase();
                // }
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
    if (sockfd < 0) {
        if ((fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)) //  to check the non blocking after tests
            std::cout << strerror(errno);
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
    bytesRecv = recv(pfds[index].fd, buffer, (BUFFSIZE - 1), 0);
    if (bytesRecv < 0) {
        std::cout << strerror(errno) << '\n';
        close(pfds[index].fd);
        pfds.erase(pfds.begin() + index);
        exit(0) ;
    }
    _body.append(buffer, bytesRecv);
    if (bytesCounter == 0) { // stor headers in a multi-map and erase them from the body
        reqmsg.assign(buffer);
        _request.get_request(reqmsg);
        _path = _request._uri;
        pos = _body.find(_request._headers); // find headers and erase theme + erase request line
        if (pos != -1) {
            // std::cout << _request._body_info_size << std::endl;
            headersSize = _request._headers.size() + pos + _request._body_info_size + 5 + 5 + 1; // 5 for the size of boundary + content disposition + content-type + \n
            // headersSize = _request._headers.size() + pos;
            _body.erase(0, headersSize);
            _request._boundary.insert((_request._boundary.size() - 1), "--");
            _request._headers.erase();
            pos = -1;
        }
    }
    bytesCounter += bytesRecv; // to accelerate the receive the data by not checking the body every time
    pos = _body.find(_request._boundary);
    if (pos  != -1) {
        // std::cout << pos << std::endl;
        // std::cout << _body.size() << std::endl;
        // _body.erase(pos, (_request._boundary.size())); // erase last boundary
        int poss = 0;
        int x;
        std::stringstream ss;
        tmpBody = ft_split(_body, "\r\n");
        for (size_t i = 0; i < tmpBody.size(); i++) {
            if ((tmpBody[i].size() > 0) && (tmpBody[i].size() < 6)) {
                try {
                    x = std::stoul(tmpBody[i], nullptr, 16);
                }
                catch (const std::exception& e)
                {
                    x = -1;
                }
                if (x != -1) {
                    sizeVec.push_back(tmpBody[i]);
                }
            }
        }
        // poss = 0;
        for (size_t i = 0; i < (sizeVec.size() - 2); i++) {
            poss = _body.find(sizeVec[i]);
            if (poss != -1) {
                x = std::stoul(sizeVec[i], nullptr, 16);
                // std::cout << x << std::endl;
                _test2 = _body.substr((poss + sizeVec[i].size() + 2), x);
                _test.append(_test2);
                _body.erase(poss, (sizeVec[i].size() + 2));
                // std::cout << sizeVec[i] << std::endl;
            }
        }
        // poss = _body.find("3a\r\n");
        // if (poss != -1)
        //     _body.erase(poss, 4);
        // poss = _body.find("0\r\n");
        // if (poss != -1)
        //     _body.erase(poss, 3);
        // _body.erase((_body.size() - 2), 2);
        // while (((pos = _body.find("\r\n")) != -1) && ((size_t)pos < _body.size()))
        // {
        //     std::cout << pos << std::endl;
        //     _body.erase(pos, 2);
        // }
        // _body.erase(_body.size() - 2);
        // std::cout << _test;
        std::ofstream filo(_request._filename);
        if (filo.is_open()) {
            filo << _test;
            filo.close();
        }
        bytesCounter = 0;
        _body.erase();
        _bodyStream.str("");
        _bodyStream.clear();
        // _request._msgrequest.clear();
        pfds[index].events = POLLOUT;
    }
}

server::~server() {}

std::vector<std::string> server::ft_split(const std::string &str, const std::string &del) {
	std::vector<std::string> res;

	std::size_t pos = 0;
	std::size_t prev = 0;
	while ((pos = str.find(del, prev)) != std::string::npos) {
		if (str.substr(prev, pos - prev) != "")
			res.push_back(str.substr(prev, pos - prev));
		prev = pos + del.size();
	}
	if (str.substr(prev) != "")
		res.push_back(str.substr(prev));
	return res;
}

// std::vector<std::string> server::ft_split(std::string const &str,  const std::string &del) {
// 	size_t start = 0;
// 	size_t end = 0;
// 	std::string key;
// 	std::vector<std::string> vec;
// 	for (std::string::size_type i = 0; i < str.size(); i++) {
// 		start = str.find(del, i);
// 		if (start == std::string::npos)
// 			break;
//         start += del.size();
// 		end = str.find(del, start);
// 		key = str.substr(start, end - start);
// 		vec.push_back(key);
// 		if (end == std::string::npos)
// 			break;
// 		i = end;
// 	}
// 	return vec;
// }

// GET METHOD
// if (_request._method == "GET") {
//     std::cout << "GET method \n";
//     if (pos_read == 0) {
//         _response.get_content_type(_path);
//         _body = _response.headers_generator(_request._status_code);
//         _body += readFile();
//         pos_read++;
//     }
//     if (_body.size() > 0) {
//         bytesCounter = _body.size() / 10;
//         if (_body.size() < BUFFSIZE)
//             bytesCounter = _body.size();
//         bytesSend = send(pfds[i].fd, _body.c_str(), bytesCounter, 0);
//         if (bytesSend < 0)
//             std::cout << strerror(errno) << '\n';
//         _body.erase(0, bytesSend);
//     }
//     else {
//         close(pfds[i].fd);
//         pfds.erase(pfds.begin() + i);
//         _body.erase();
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




// NORMAL TRANSFER (RECEIVE) OF DATA "POST METHOD"
    // memset(buffer, 0, BUFFSIZE);
    // bytesRecv = recv(pfds[index].fd, buffer, (BUFFSIZE - 1), 0);
    // if (bytesRecv < 0) {
    //     std::cout << strerror(errno) << '\n';
    //     close(pfds[index].fd);
    //     pfds.erase(pfds.begin() + index);
    //     exit(0) ;
    // }
    // _body.append(buffer, bytesRecv);
    // if (bytesCounter == 0) { // stor headers in a multi-map and erase them from the body
    //     reqmsg.assign(buffer);
    //     _request.get_request(reqmsg);
    //     _path = _request._uri;
    //     pos = _body.find(_request._headers); // find headers and erase theme + erase request line 
    //     if (pos != -1) {
    //         headersSize = _request._headers.size() + pos + _request._body_info_size + 5;  5 for the size of boundary + content disposition + content-type + \n
    //         _body.erase(0, headersSize);
    //         _request._boundary.insert((_request._boundary.size() - 1), "--");
    //         pos = -1;
    //     }
    // }
    // bytesCounter += bytesRecv; // to accelerate the receive the data by not checking the body every time
    // if ((bytesCounter + headersSize) > _request._content_length)
    //     pos = _body.find(_request._boundary);
    // if (pos != -1) {
    //     _body.erase(pos, (_request._boundary.size())); // erase last boundary
    //     std::ofstream filo(_request._filename);
    //     if (filo.is_open()) {
    //         filo << _body;
    //         filo.close();
    //     }
    //     bytesCounter = 0;
    //     _body.erase();
    //     pfds[index].events = POLLOUT;
    // }











