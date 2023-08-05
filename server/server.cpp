#include "server.hpp"

bool server::serverGetaddrinfo(std::string &_port, std::string &_host) {
	// void for the moment after that i must pass the result of the parser <multimap>
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	if ((getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result)) < 0) {
		std::cerr <<  gai_strerror(errno);
		return (false);
	}
	return (true);
}

int server::serverSocket(void) {
	_socketFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_socketFd < 0) {
		std::cerr << strerror(errno);
		exit (1);
	}
	// listners.push_back(_socketFd);
	return (_socketFd);
}

bool server::serverSetsocket(void) {
	int setsock = 1;
	if ((setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
		std::cerr << strerror(errno);
		return (false);
	}
	fcntl(_socketFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
	return (true);
}

bool server::serverBind(void) {
	if ((bind(_socketFd, result->ai_addr, result->ai_addrlen)) < 0) {
		std::cerr << strerror(errno);
		return (false);
	}
	return (true);
}

bool server::serverListen(void) {
	if ((listen(_socketFd, SOMAXCONN)) < 0) {
		std::cerr << strerror(errno);
		return (false);
	}
	return (true);
}

int server::serverPoll(void) {
	_totalFds = poll(&pfds[0], pfds.size(), -1);
	if (_totalFds < 0) {
		std::cerr << strerror(errno);
	}
	return (_totalFds);
}

int server::serverAccept(int fd) {
	_clientaddrln = sizeof(_client);
	_clinetFd = accept(fd, (struct sockaddr*)&_client, &_clientaddrln);
	fcntl(_clinetFd, F_SETFL, O_NONBLOCK); //  to check the non blocking after tests
	if (_clinetFd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			std::cerr << strerror(errno) << " error \n";
		else {

			std::cerr << strerror(errno) << "hamid \n";
		}
	}
	else 
		addFDescriptor(_clinetFd); // fill the pfds vector
	return (_clinetFd);
}

void server::addFDescriptor(int fd) {
	pollfds.fd = fd;
	if (pollfds.fd == _socketFd)
		pollfds.events = POLLIN | POLLHUP;
	else
		pollfds.events = POLLIN | POLLOUT | POLLHUP;
	pfds.push_back(pollfds);
}

server::server(std::multimap<std::string, std::pair<std::string, std::string> >const & m, ConfigFileParser const & _File) :_configFile(_File)  {
	
	for (std::multimap<std::string, std::pair<std::string, std::string> >::const_iterator it = m.begin() ; it != m.end(); it++) {
	
		t_socketListner sock;

		sock._host = it->second.first;
		sock._port = it->first;
		sock._serverName = it->second.second;
		
		if (!serverGetaddrinfo(sock._port, sock._host))
			exit(1);
		if (!serverSocket())
			exit(1);
		if (!serverSetsocket()) 
			exit(1);
		if (!serverBind()) // in case of error must close the listner
			exit(1);
		freeaddrinfo(result); 
		if(!serverListen())
			exit(1);
		addFDescriptor(_socketFd);

		_listnersMap.insert( std::pair<int, t_socketListner>(_socketFd, sock) );
	}
	
	for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++) {
	}

	while (1) {
		_totalFdsCheck = serverPoll();
		if (_totalFdsCheck < 0)
			break ; // poll call failed
		for (size_t i = 0; i < pfds.size(); i++) {
			if (pfds[i].revents & POLLIN) {
				if (i < _listnersMap.size()) {
					for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++) { // serch for the sokcet that contains event
						
						if (pfds[i].fd == _listnersIt->first) { // Creation of a new client
							std::cout << "new client" << std::endl;
							client _clientObj; 
							_newClientFd = serverAccept(pfds[i].fd);   // Listening descriptor is readable.
							_clientObj.setFd(_newClientFd);
							_clientObj.enableStartRecv();
							_clientObj._port = _listnersIt->second._port;
							_clientObj._host = _listnersIt->second._host;
							_clientObj._hostname = _listnersIt->second._serverName;
							_clientsMap.insert( std::pair<int, client>(_newClientFd, _clientObj) );
							break;
						}

					}
				}
				else {
					serverReceive(pfds[i].fd, i); // check event for receive
				}
			}
			else if (pfds[i].revents & POLLOUT) { // check event for send
				serverSend(pfds[i].fd, i);
			}
			else if (pfds[i].revents & POLLHUP) {
				std::cerr << "CLIENT GOT PROBLEMS" << std::endl;
			}
		}
	}
}

void server::serverReceive(int fd, int index) {
	_mapIt = _clientsMap.find(fd);
	if (_mapIt != _clientsMap.end()) {
		
		memset(_buffer, 0, BUFFSIZE);
		_bytesRecv = recv(_mapIt->second._fd, _buffer, BUFFSIZE, 0);
		if (_bytesRecv < 0) {
			close(fd);
			pfds.erase(pfds.begin() + index);  
			_clientsMap.erase(_mapIt);
			return ;
		}
		
		_mapIt->second._requestBody.append(_buffer, _bytesRecv);
		if (_mapIt->second._startRecv) { // stor headers in a multi-map and erase them from the body
			std::cout << _mapIt->second._requestBody << std::endl;
			_request.requestHeader(_mapIt->second._requestBody);
			_response.code = 200;
			_URI = _request._URI ;
			_mapIt->second.disableStartRecv();
			if (_request._method == "POST")
			    _request.erasePostRequestHeaders(_mapIt->second);
		}

		// GET CURENT SERVER
		getCurrentServer(_mapIt->second);


		// GET LOCATION
		if(!LocationAvilability()) {
			std::cout << "location not found" << std::endl;
			_response.code = 404;
		}
		// IS LOCATION HAVE A RETURN REDIRECTION
		else if (!RedirectionAvilability()) {
			std::cout << "redirecti zmer" << std::endl;
			_response.code = 301;
		}
		// METHODS ALLOWED IN LOCATION
		else if (!AllowedMethods()) { // WHAT METHODES AR ALLOWED
			std::cout << "ma method ma pikala" << std::endl;
			_response.code = 405;
		}
		else {
			// SERVE NOW THE CONTENT DEPEND ON THE REQUEST METHOD AND THE ALLOWED ONE ON LOCATION
			if (_request._method == "GET" ) { 
				// GET RESOURCE TYPE
				getResourceType(); // IF CLIENT REQUEST A FILE OR DIRECTORY

				if (_isDirectory) {
					std::cout << "to response" << std::endl;
					serveDirecotry(_mapIt->second);
					if (!_mapIt->second._isIndexFile) {
						_mapIt->second._autoIndex = true;
						std::cout << "content listed for client: " << _mapIt->second._fd << std::endl;
					}
					else {
						_mapIt->second._autoIndex = false; // To check if it's about an autoindex or not
						// _response._path = _URI; // MUST UPDATE THE RESPONSE PATH
						std::cout << "new path: " << _response._path << std::endl;
					}
				}
				else {
					std::cout << "enable start send" << std::endl;
					if (_response.code != 404 && _response.code != 405)
						UriAvilability(_mapIt->second);
				}
			}
			else if (_request._method == "POST") {
				
			    _request.postMethod(_mapIt->second);
			}
			// else if (_request._method == "DELETE") {
			//     std::remove(_request._URI.c_str());
			//     _mapIt->second._requestBody.clear();
			//     _mapIt->second.enableStartSend();
			// }
		}
		std::cout << _response.code << std::endl;
		_mapIt->second.enableStartSend();
	}
}

void server::getCurrentServer(client &_client) {
	_serverIndex = -1;
	std::cout << _client._port << std::endl;
	for (size_t i = 0; i < _configFile.GetNumberOfServers(); i++) { // GET THE SERVER
		for (size_t j = 0; j < _configFile.GetServers()[i].GetPortNumbers().size(); j++) {
			if (_client._port == _configFile.GetServers()[i].GetPortNumbers()[j]){
				_serverIndex = i;
				break;
			}
		}
		if (_serverIndex != -1)
			break; // TO BREAK THE SECOND LOOP
	}
}

bool server::LocationAvilability(void) {
	std::string tmp;
	int         position;

	position = -1;
	_locationIndex = -1;
	for (size_t i = 0; i < _configFile.GetServers()[_serverIndex].GetLocationContexts().size(); i++) {
		tmp = _configFile.GetServers()[_serverIndex].GetLocationContexts()[i].GetLocationUri().GetUri();
		if (((position = _URI.find(tmp, 0)) == 0) && tmp.size() > 1) {
			_locationIndex = i;
			return (true);
		}
	}
	return (false);
}

bool server::RedirectionAvilability(void) {
	std::string returnUri;

	returnUri = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetReturn().GetUrl();
	if (returnUri.size() > 0) {
		std::cout << "redirection url exist" << std::endl;
		_response._path = returnUri;
		return (false);
	}
	return (true);
}

bool server::AllowedMethods(void) {

	if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetGET()) {
		std::cout << "GET is there" << std::endl;
		return (true);
	}
	return (false);
}

void server::getResourceType(void) {
	std::string tmpURI;
	DIR *directory;

	if (_configFile.GetServers()[_serverIndex].GetRoot().size() > 0)
		tmpURI = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot();
	else
		tmpURI = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot().size();
	std::cout << tmpURI << std::endl;
	if (tmpURI.size() > 0) {
		tmpURI.append(_URI);
		std::cout << tmpURI << std::endl;
		if ((directory = opendir(tmpURI.c_str())) != nullptr) { // CHECK THE RESOURCE REQUESTED IF IT IS AN AVILABLE DIRECTORY
			std::cout << "it's a folder" << std::endl;
			_isDirectory = true;
			closedir(directory);
		}
		else {
			std::cout << "it's a file" << std::endl;
			_isDirectory = false;
			_mapIt->second._isIndexFile = false;
			_mapIt->second._autoIndex = false;
			_response._path = tmpURI;
		}
	}
}

std::string server::AppendRootAndUri(void) {
	std::string tmpURI;

	tmpURI = (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot());
	std::cout << "==>" << _URI << std::endl;
	if (_URI.find(tmpURI) != std::string::npos)
		tmpURI.append(_URI);
	return (tmpURI);
}

void server::serveDirecotry(client &_client) {
	std::string tmp;
	std::string root;
	size_t      size;
	DIR         *directory;
	struct dirent *entry;

	size =  _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex().size();
	_client._defaultIndexFiles = false;
	// IF THE LOCATION HAVE INDEX FILES
	if (size == 4) { 
		tmp = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[3];
		// INDEX IN THE DEFAULT MODE SO CHECK FOR AUTOINDEX
		if (tmp == "index.nginx-debian.html")
			_client._defaultIndexFiles = true;
	}
	root = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot();
	// CHECK FOR INDEXES FILES
	if (!_isDirectory) {
		_client._isIndexFile = false;
		for (size_t i = 0; i < size; i++) {
			tmp = root;
			if (_URI.find('/', (_URI.size() - 1)) == std::string::npos)
				_URI.append("/");
			tmp.append(_URI);
			tmp.append(_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[i]);
			std::cout << tmp << std::endl;
			if (access(tmp.c_str(), F_OK | R_OK | W_OK ) == -1) {
				std::cout << "error access path" << std::endl;
				_response.code = 404;
			}
			else {
				_response.code = 200;
				_response._path = tmp;
				_client._isIndexFile = true;
				break ;
			}
		}
	}
	// IF AUATOINDEX ON
	else if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetAutoIndexDir()) {
			std::string listFile;
			
			tmp = root;
			tmp.append(_URI);
			if (_URI.find('/', (_URI.size() - 1)) == std::string::npos)
				_URI.append("/");
			std::cout << "autoindex on" << std::endl;
			std::cout << "current path: " << tmp << std::endl;

			if ((directory = opendir(tmp.c_str())) != nullptr) {
				_response._locationContent.clear();
				while ((entry = readdir(directory)) != NULL) {
					std::cout << entry->d_name << std::endl;
					listFile = "<p> <a style=\"color:red;\"";
					listFile += "href=\"http://";
					listFile += _client._hostname;
					listFile += ":";
					listFile += _client._port;
					listFile += _URI;
					listFile += entry->d_name;
					listFile += "\">";
					listFile += entry->d_name;
					listFile += "</a></p>\n";
					_response._locationContent += listFile;
				}
				listFile.clear();
				closedir(directory);
			}
	}
	// IF AUTOINDEX OFF
	else if (!_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetAutoIndexDir()) {
		std::cout << "autoindex off" << std::endl;
		_response.code = 403;
	}
}

void server::UriAvilability(client &_client) {
	_client._autoIndex = false;
	// if (!_client._autoIndex) {
		if (_response.code == 301) {
			_response._path = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetReturn().GetUrl();
			std::cout << _response._path << std::endl;
			return ;
		}
		if (access(_response._path.c_str(), F_OK | R_OK | W_OK ) == -1) {
			std::cout << "error access path" << std::endl;
			_response.code = 404;
		}
	// }
}

void server::serverSend(int fd, int index) {
	_mapIt = _clientsMap.find(fd);
	if (_mapIt != _clientsMap.end()) {

		if (_request._method == "GET") {
			if (_response.getMethodResponse(_mapIt->second)) {
				if (_request._connection != "keep-alive\r") {
					close(_mapIt->second._fd);
					pfds.erase(pfds.begin() + index);
					_clientsMap.erase(_mapIt);
				}
				else
					_mapIt->second.enableStartRecv();
				_request._method.clear();
				_URI.clear();
			}
		}
		// else if ((_request._method == "POST") && _mapIt->second._startSend) {
		//     _response.postMethodResponse(_mapIt->second);
		//     if (_request._connection != "keep-alive\r") {
		//         close(_mapIt->second._fd);
		//         pfds.erase(pfds.begin() + index);
		//         _clientsMap.erase(_mapIt);
		//     }
		//     _mapIt->second.enableStartRecv();
		// }
		// else if (_request._method == "DELETE") {
		//     _response.deleteMethodResponse(_mapIt->second);
		//     if (_request._connection != "keep-alive\r") {
		//         close(_mapIt->second._fd);
		//         pfds.erase(pfds.begin() + index);
		//         _clientsMap.erase(_mapIt);
		//     }
		//     _mapIt->second.enableStartRecv();
		// }

	}
}

server::~server() {}