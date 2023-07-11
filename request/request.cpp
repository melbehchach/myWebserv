#include "request.hpp"

request::request() {

}

void request::get_request(std::string& headers) {
    _message << headers;
    check_request_line();
    get_headers();
    get_body_info();
    _message.str("");
    _message.clear();
    // std::cout << "[ REQUEST ]\n";
    // for (_it = _msgrequest.begin(); _it != _msgrequest.end(); _it++)
    //     std::cout << (*_it).first << (*_it).second << '\n';
}

bool request::check_request_line(void) { // reading the request line to get infos about http method uri and http version
    std::string tmp;
    int         pos;

    _status_code = 200;
    std::getline(_message, tmp, '\n');
    for (int i = 0; i < 3; i++) {
        pos = tmp.find(' ');
        if (i == 0)
            _method = tmp.substr(0, (pos));
        else if (i == 1)
            _uri = tmp.substr(0, (pos));
        else
            _http = tmp.substr(0, (tmp.size() - 1));
        if (pos != -1) 
            tmp.erase(0, (pos + 1));
    }
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        _status_code = 405;
        return (false);
    }
    if (_uri.size() > 2048U) {
        _status_code = 414;
        return (false);
    }
    if (access(_uri.c_str(), F_OK | R_OK) != 0) {
        _status_code = 403;
        return (false);
    }
    if (_http != "HTTP/1.1") {
        _status_code = 505;
        return (false);
    }
    return (true);
}

void request::get_headers(void) {
    std::string tmp;
    int         size;
    int         pos;

    while (1) {
        std::getline(_message, tmp, '\n');
        _headers.append(tmp);
        _headers += '\n';
        if (tmp == "\r")
            break;
        size = tmp.size();
        pos = tmp.find(':');
        if (pos == -1) // in case of header contains an empty line
            continue;
        _key = tmp.substr(0, (pos + 1));
        _value = tmp.substr((pos + 2), (size - pos));
        if (_key == "Content-Length:") {
            std::istringstream iss(_value);
            iss >> _content_length;
        }
        else if (_key == "Connection" )
            _connexion = _value;
        _msgrequest.insert( std::pair<std::string, std::string>(_key, _value));
    }
}

void request::get_body_info(void) {
    std::string tmp;
    int         size;
    int         pos;

    _it = _msgrequest.find("Transfer-Encoding:");
    if (_it != _msgrequest.end()) {
        _transfer = true;
        std::getline(_message, tmp, '\n');
    }
    else
        _transfer = false;
    for (int i = 0; i < 3; i++) {
        std::getline(_message, tmp, '\n');
        size = tmp.size();
        _body_info_size += size;
        if (i == 0)
            _boundary = tmp;
        else if (i == 1) {
             if ((pos = tmp.find("filename=")) != -1)
                _filename = tmp.substr((pos + 10), (size - (pos + 12)));
        }
        else {
            pos = tmp.find(':');
            _content_type = tmp.substr((pos + 2), (size - pos));
        }
    }
}

void request::_ParseRequestHeaders(std::string& tmpBody) {
    get_request(tmpBody); // get request infos
    position = tmpBody.find(_headers); // find headers and erase theme && position = request line length
    if (position != -1) {
        headersSize = _headers.size() + position + _body_info_size + 5 + 1; // 5 for the size of boundary + content disposition + content-type + \n
        tmpBody.erase(0, headersSize);
        _boundary.insert((_boundary.size() - 1), "--");
        _headers.erase();
        position = -1;
    }
}

void    request::_parseChunkedRequestBody(std::string &tmpBody) {
    _chunksVector = ft_split(tmpBody, "\r\n");
    for (size_t i = 0; i < _chunksVector.size(); i++) {
        if ((_chunksVector[i].size() > 0) && (_chunksVector[i].size() < 6)) {
            try {
                _chunkSize = std::stoul(_chunksVector[i], nullptr, 16);
            }
            catch (const std::exception& e) {
                _chunkSize = -1;
            }
            if (_chunkSize != -1)
                _chunkSizeVector.push_back(_chunksVector[i]);
        }
    }
    for (size_t i = 0; i < (_chunkSizeVector.size() - 2); i++) { // _chunkSizeVector.size() - 2 because the last 2 elements are 3a => for the boundry size and 0 size of last chunk
        position = tmpBody.find(_chunkSizeVector[i]); // find size of the chunk in the body
        if (position != -1) {
            _chunkSize = std::stoul(_chunkSizeVector[i], nullptr, 16);
            _chunks = tmpBody.substr((position + _chunkSizeVector[i].size() + 2), _chunkSize);
            _body.append(_chunks);
            tmpBody.erase(position, (_chunkSizeVector[i].size() + 2));
        }
    }
    std::ofstream file(_filename);
    if (file.is_open()) {
        file << _body;
        file.close();
    }
    _body.erase();
    _msgrequest.clear();
    _chunksVector.clear();
    _chunkSizeVector.clear();
}

void    request::_parseNormalRequestBody(std::string &tmpBody, int boundaryPosition) {
    tmpBody.erase(boundaryPosition, (_boundary.size() + 4));
    _body.append(tmpBody);
    std::ofstream file(_filename);
    if (file.is_open()) {
        file << _body;
        file.close();
    }
    _body.erase();
    _msgrequest.clear();
}


std::vector<std::string> request::ft_split(const std::string &str, const std::string &del) {
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

request::~request() {}