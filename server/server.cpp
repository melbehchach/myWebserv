#include "server.hpp"

bool server::serverGetaddrinfo(std::string &_port, std::string &_host) {
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	if ((getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result)) < 0) {
		std::cerr << gai_strerror(errno);
		return (false);
	}
	return (true);
}

int server::serverSocket(void) {
	_socketFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_socketFd < 0) {
		std::cerr << strerror(errno);
		exit(1);
	}
	return (_socketFd);
}

bool server::serverSetsocket(void) {
	int setsock = 1;
	if ((setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&setsock, sizeof(setsock))) < 0) {
		std::cerr << strerror(errno);
		return (false);
	}
	fcntl(_socketFd, F_SETFL, O_NONBLOCK);
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
	_clinetFd = accept(fd, (struct sockaddr *)&_client, &_clientaddrln);
	fcntl(_clinetFd, F_SETFL, O_NONBLOCK);
	if (_clinetFd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			std::cerr << strerror(errno) << std::endl;
	}
	else
		addFDescriptor(_clinetFd);
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

server::server(std::multimap<std::string, std::pair<std::string, std::string> > const &m, ConfigFileParser const &_File) : _configFile(_File) {
	for (std::multimap<std::string, std::pair<std::string, std::string> >::const_iterator it = m.begin(); it != m.end(); it++) {
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
		if (!serverBind())
			exit(1);
		freeaddrinfo(result);
		if (!serverListen())
			exit(1);
		addFDescriptor(_socketFd);

		_listnersMap.insert(std::pair<int, t_socketListner>(_socketFd, sock));
	}

	for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++)
	{
	}
	while (1) {

		_totalFdsCheck = serverPoll();
		if (_totalFdsCheck < 0)
			break; 
		for (size_t i = 0; i < pfds.size(); i++) {
			if (pfds[i].revents & POLLIN) {
				if (i < _listnersMap.size()) {
					for (_listnersIt = _listnersMap.begin(); _listnersIt != _listnersMap.end(); _listnersIt++) { 
						if (pfds[i].fd == _listnersIt->first) {
							client _clientObj;
							_newClientFd = serverAccept(pfds[i].fd); 
							_clientObj.setFd(_newClientFd);
							_clientObj.enableStartRecv();
							_clientObj._port = _listnersIt->second._port;
							_clientObj._host = _listnersIt->second._host;
							_clientObj._hostname = _listnersIt->second._serverName;
							_clientsMap.insert(std::pair<int, client>(_newClientFd, _clientObj));
							break;
						}
					}
				}
				else {
					serverReceive(pfds[i].fd, i); 
				}
			}
			else if (pfds[i].revents & POLLOUT) {
				serverSend(pfds[i].fd, i);
			}
			else if (pfds[i].revents & POLLHUP) {
				std::cout << "Error in client" << std::endl;
			}
		}
	}
}

void server::serverReceive(int fd, int index) {
	_mapIt = _clientsMap.find(fd);
	if (_mapIt != _clientsMap.end()) {
		memset(_buffer, 0, BUFFSIZE);
		_bytesRecv = recv(_mapIt->second._fd, _buffer, BUFFSIZE, 0);
		if (_bytesRecv <= 0) {
			close(fd);
			pfds.erase(pfds.begin() + index);
			_clientsMap.erase(_mapIt);
			return;
		}
		_mapIt->second._requestBody.append(_buffer, _bytesRecv);
		if (_mapIt->second._startRecv) {
			if (_mapIt->second._requestBody.size() > 1){
				_request.requestHeader(_mapIt->second._requestBody);
				_mapIt->second._method = _request._method;
				_URI = _request._URI;
				_firstResourceCheck = true;
				_mapIt->second.disableStartRecv();
				_response.code = 200;
				if (_request._method == "POST")
					_request.erasePostRequestHeaders(_mapIt->second);

				getCurrentServer(_mapIt->second);
				if (!LocationAvilability()) {
					if (access(_URI.c_str() , R_OK | W_OK) == -1) {
						if (!errorPageChecker(404, _mapIt->second))
							_response.code = 404;
						_mapIt->second.enableStartSend();
						return;
					}
				}
				else {
					if (!RedirectionAvilability()) {
						_response.code = 301;
						_mapIt->second.enableStartSend();
						return;
					}
					if (!AllowedMethods()) {
						if (!errorPageChecker(405, _mapIt->second))
							_response.code = 405;
						_mapIt->second.enableStartSend();
						return;
					}
				}
			}
		}
		if (_response.code == 200) {
			if (_request._method == "POST")
				postMethod(_mapIt->second);
			else
			{
				if (_request._method == "GET")
					getMethod(_mapIt->second);
				else if (_request._method == "DELETE")
					deleteMethod(_mapIt->second);
			}
		}
		_mapIt->second.enableStartSend();
	}
}

void server::getCurrentServer(client &_client) {
	_serverIndex = -1;
	for (size_t i = 0; i < _configFile.GetNumberOfServers(); i++) {
		for (size_t j = 0; j < _configFile.GetServers()[i].GetPortNumbers().size(); j++) {
			if (_client._port == _configFile.GetServers()[i].GetPortNumbers()[j]) {
				_serverIndex = i;
				break;
			}
		}
		if (_serverIndex != -1)
			break;
	}
}

bool server::LocationAvilability(void) {
	std::string tmp;
	int position;

	position = -1;
	_locationIndex = -1;
	for (size_t i = 0; i < _configFile.GetServers()[_serverIndex].GetLocationContexts().size(); i++) {	
		tmp = _configFile.GetServers()[_serverIndex].GetLocationContexts()[i].GetLocationUri().GetUri();
		std::cout << tmp << "\n";
		if ((_URI.size() == 1) && ((position = tmp.find(_URI.c_str(), 0, tmp.size())) != -1)) {
			_locationIndex = i;
			return (true);
		}
		else if (((position = tmp.find(_URI.c_str(), 0, tmp.size())) != -1) && (tmp.size() > 1)) {
			_locationIndex = i;
			return (true);
		}
	}
	return (false);
}

bool server::RedirectionAvilability(void) {
	std::string returnUri;

	if (_locationIndex != -1)
		returnUri = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetReturn().GetUrl();
	else
		returnUri = _configFile.GetServers()[_serverIndex].GetReturn().GetUrl();
	if (returnUri.size() > 0)
	{
		_response._path = returnUri;
		return (false);
	}
	return (true);
}

bool server::AllowedMethods(void) {
	if (_request._method == "GET"){
		if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetGET())
			return (true);
		else
			return (false);
	}
	else if (_request._method == "POST"){
		if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetPost())
			return (true);
		else
			return (false);
	}
	else if (_request._method == "DELETE"){
		if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetMethods().GetDelete())
			return (true);
		else
			return (false);
	}
	return (false);
}

bool server::errorPageChecker(int code, client &_client) {
	std::map<int, std::string> tmp;
	std::map<int, std::string>::iterator errorMapIt;
	if (_serverIndex != -1 && _locationIndex != -1 ) {
		tmp = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetErrorPage();
	}
	else if (_locationIndex == -1) {
		tmp = _configFile.GetServers()[_serverIndex].GetErrorPage();
	}

	errorMapIt =  tmp.find(code);
	if (errorMapIt !=  tmp.end()) {
		_response._path = errorMapIt->second;
		_response.code = errorMapIt->first;
		_client._errorPageExist = true;
	}
	return (_client._errorPageExist);
}

void server::postMethod(client &_client) {
	if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetCmbs() > 0)
		_client._clientBodySize = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetCmbs();

	
	if (_firstResourceCheck)
		getResourceType(_client);
	if (_isDirectory) {
		if (IndexExist()) {
			if (!runCgi(_client) && _response.code == 200) {
				_isDirectory = false;
			}
		}
	}
	else
		_request.postMethod(_client);
}

void server::getMethod(client &_client) {
	getResourceType(_client);
	if (_isDirectory) {
		if (IndexExist()) {
			if (!runCgi(_client) && _response.code == 200)
				ServeIndexFile(_client);
		}
		else
			serveDirecotry(_client);
	}
	else {
		if (!runCgi(_client)) {
			if (_response.code != 404 && _response.code != 405)
				UriAvilability(_client);
		}
	}
}

void server::deleteMethod(client &_client) {
	getResourceType(_client);
	if (_isDirectory)
		deleteLocation(_client);
	else
		deleteFile(_client);
}

void server::getResourceType(client &_client) {
	std::string tmpURI;
	DIR *directory;

	if (access(_URI.c_str() , R_OK | W_OK) != -1) {
		_isDirectory = false;
		return ;
	}

	if (_configFile.GetServers()[_serverIndex].GetRoot().size() > 0)
		tmpURI = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot();
	else
		tmpURI = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot().size();
	if (tmpURI.size() > 0) {
		if (_URI.find('?') == std::string::npos)
			tmpURI.append(_URI);
		else {
			this->_Query = _URI.substr(_URI.find('?') + 1);
			_URI.erase(_URI.find('?'));
			tmpURI.append(_URI);
		}
		if ((directory = opendir(tmpURI.c_str())) != nullptr) {
			_isDirectory = true;
			_pathForDelete = tmpURI;
			_client._uploadPath = tmpURI;
			closedir(directory);
		}
		else {
			_isDirectory = false;
			_response._path = tmpURI;
		}
	}
	_firstResourceCheck = false;
}

bool server::IndexExist(void) {
	std::string tmp;
	std::string root;
	size_t size;

	_IndexFiles = true;
	size = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex().size();
	if (size == 4) {
		tmp = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[3];
		if (tmp == "index.nginx-debian.html")
			_IndexFiles = false;
	}
	return (_IndexFiles);
}


bool server::runCgi(client &_client) {

	if (access(_URI.c_str() , R_OK | W_OK) != -1) {
		_client._cgiOn = false;
		return (_client._cgiOn);
	}

	if (this->_Query.compare("") != 0  && _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetCGI().GetFilePath().compare("") != 0) {
		cgiData input(_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex],
			_configFile.GetServers()[_serverIndex],
				this->_request);
			std::ostringstream ss;
		try {
			CGI cgi(input, _client._port, this->_Query);
			this->parseCgiOutput(cgi.GetOutput(), ss, cgi.GetExtention());
		}
		catch (std::exception const & e) {
			if (!errorPageChecker(500, _client))
				_response.code = 500;
			_client._cgiOn = false;
			std::cout << e.what() << "\n";
			return (_client._cgiOn);
		}
		_client._cgiOn = true;
		_client._responseBody.append(ss.str());
	}
	else if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetCGI().GetFilePath().compare("") != 0) {
			cgiData input(_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex],
			_configFile.GetServers()[_serverIndex],
				this->_request);
			std::ostringstream ss;
		try {
			CGI cgi(input, _client._port, _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[0]);
			this->parseCgiOutput(cgi.GetOutput(), ss, cgi.GetExtention());
		}
		catch (std::exception const & e) {
			if (!errorPageChecker(500, _client))
				_response.code = 500;
			_client._cgiOn = false;
			std::cout << e.what() << "\n";
			return (_client._cgiOn);
		}
		_client._cgiOn = true;
		_client._responseBody.append(ss.str());
	}
	return (_client._cgiOn);
}

void server::ServeIndexFile(client &_client) {
	std::string tmp;
	std::string root;
	size_t 		size;

	size = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex().size();
	root = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot();
	for (size_t i = 0; i < size; i++) {
		tmp = root;
		if (_URI.find('/', (_URI.size() - 1)) == std::string::npos)
			_URI.append("/");
		tmp.append(_URI);
		tmp.append(_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetIndex()[i]);
		if (access(tmp.c_str(), R_OK | W_OK) == -1) {
			if (!errorPageChecker(404, _client))
				_response.code = 404;
		}
		else {
			_response.code = 200;
			_response._path = tmp;
			break;
		}
	}
}

void server::serveDirecotry(client &_client) {
	std::string tmp;
	std::string root;
	DIR *directory;
	struct dirent *entry;

	root = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetRoot();
	if (_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetAutoIndexDir()) {
		std::string listFile;

		tmp = root;
		tmp.append(_URI);
		if (_URI.find('/', (_URI.size() - 1)) == std::string::npos)
			_URI.append("/");

		if ((directory = opendir(tmp.c_str())) != nullptr) {
			_response._locationContent.clear();
			while ((entry = readdir(directory)) != NULL) {
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
			_client._autoIndexOn = true;
		}
	}
	else if (!_configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetAutoIndexDir()) {
		if (!errorPageChecker(403, _client))
			_response.code = 403;
	}
}

void server::UriAvilability(client &_client) {
	if (_response.code == 301) {
		_response._path = _configFile.GetServers()[_serverIndex].GetLocationContexts()[_locationIndex].GetReturn().GetUrl();
		return ;
	}
	if (access(_URI.c_str(), R_OK | W_OK) == -1) {
		if (!errorPageChecker(404, _client))
			_response.code = 404;
	}
	else {
		_response.code = 200;
		_response._path = _URI;
	}
	std::cout << _response.code << std::endl;
}

void server::deleteFile(client &_client) {
	if (access(_response._path.c_str(), F_OK) == -1) {
		std::cout << _response._path.c_str() << "\n";
		std::cout << "REPOSNE PATH VALIDE" << std::endl;
		if (!errorPageChecker(500, _client))
			_response.code = 500;
	}
	else {
		if (std::remove(_response._path.c_str()) == 0)
			_response.code = 204;
		else {
			if (!errorPageChecker(500, _client))
				_response.code = 500;
		}
	}
}

int server::deleteDirectoryContent(std::string const path) {
	DIR *dir;
	dirent *entry;
	std::string filePath;

	dir = opendir(path.c_str());
	if (!dir)
		return (500);

	while ((entry = readdir(dir)) != NULL) {
		// SKIP . AND ..
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		filePath = path + "/" + entry->d_name;
		if (access(_response._path.c_str(), F_OK ) == -1)
			_response.code = 500;
		else {
			struct stat fileStat;
			if (lstat(filePath.c_str(), &fileStat) == -1) {
				return (500);
				continue;
			}
			if (S_ISDIR(fileStat.st_mode)) {
				deleteDirectoryContent(filePath);
			}
			else {
				std::remove(filePath.c_str());
			}
		}
	}
	closedir(dir);
	return (deleteSubDirectories(path));
}

int server::deleteSubDirectories(std::string const path) {
	DIR *dir;
	dirent *entry;
	std::string filePath;

	dir = opendir(path.c_str());
	if (!dir) {
		return (500);
	}
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		filePath = path + "/" + entry->d_name;
		if (rmdir(filePath.c_str()) != 0) {
			return (500);
		}
	}
	return (204);
}

void server::deleteLocation(client &_client) {
	int ret;

	ret = deleteDirectoryContent(_pathForDelete);
	if (ret == 500) {
		if (!errorPageChecker(ret, _client))
			_response.code = ret;
	}
	else
		_response.code = ret;
}

void server::serverSend(int fd, int index) {

	_mapIt = _clientsMap.find(fd);
	if (_mapIt != _clientsMap.end()) {
		if (_mapIt->second._startSend) {
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
			else if (_request._method == "DELETE") {
				_response.deleteMethodResponse(_mapIt->second);
				if (_request._connection == "close\r") {
					close(_mapIt->second._fd);
					pfds.erase(pfds.begin() + index);
					_clientsMap.erase(_mapIt);
				}
				else
					_mapIt->second.enableStartRecv();
				_request._method.clear();
				_URI.clear();
			}
			else if ((_request._method == "POST") && _mapIt->second._startSend) {
				if (_mapIt->second._errorCode == 413)
					_response.code = 413;
				_response.postMethodResponse(_mapIt->second);
				if (_request._connection != "keep-alive\r") {
					close(_mapIt->second._fd);
					pfds.erase(pfds.begin() + index);
					_clientsMap.erase(_mapIt);
				}
				else {
					_mapIt->second.enableStartRecv();
				}
			}
		}
		_request._msgrequest.clear();
	}
}

server::~server() {}


void server::parseCgiOutput(std::string &input, std::ostringstream &header, std::string const &ex) {
	std::istringstream s(input);
	std::string buff;
	time_t raw;
	std::string tm;
	time(&raw);
	tm = ctime(&raw);
	tm.pop_back();
	std::string body;
	std::string connection = this->_request._connection;
	header << this->_response.statusLine() << "Server: WebServ\r\n" << "Date: " << tm << " GMT\r\n" ; //  << "Connection: " << connection << "\r\n"; Mark: this is the error! Connection in response alo ?? 
	if (ex.compare(".php") == 0) {
		while (std::getline(s, buff)) {
			if (buff.find("X-Powered-By:") != std::string::npos) {
				header << "X-Powered-By: "  << buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.find("Set-Cookie:") != std::string::npos){
				header << "Set-Cookie: " <<  buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.find("Expires:") != std::string::npos){
				header << "Expires: " << buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.find("Cache-Control:") != std::string::npos) {
				header << "Cache-Control: " << buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.find("Pragma:") != std::string::npos) {
				header << "Pragma: " << buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.find("Content-type:") != std::string::npos) {
				header << "Content-type: " <<  buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
			}
			else if (buff.compare("\r\n\r\n") == 0)
				break;
		}
		if (input.find("\r\n\r\n") != std::string::npos)
			body = input.substr(input.find("\r\n\r\n") + 4);
		else {
			body = input;
		}
		header << "Content-Length: " + (std::to_string(body.size()));
	}
	else if (ex.compare(".py") == 0) {
		while (std::getline(s, buff)) {
			if (buff.find("Content-type:") != std::string::npos)
				header << "Content-type: " << buff.substr(buff.find(": ") + 2, (buff.find_first_of("\r")) - ( buff.find(": ") + 2 )) << "\r\n";
		}
		if (input.find("\r\n\r\n") != std::string::npos)
			body = input.substr(input.find("\r\n\r\n") + 4);
		else {
			body = input;
		}
		header << "Content-Length: " + std::to_string(input.size());
	}
	header << "\r\n\r\n";
	header << body;
}

