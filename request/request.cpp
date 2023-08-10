#include "request.hpp"

request::request() {
    _multipleFiles = false;
    _chunkedTransfer = false;
}

void request::requestHeader(std::string& buffer) {
    _message << buffer;
    _headers.clear();
    getRequestLine();
    getHeaders();
    _position = buffer.find(_headers);
    std::cout << _headers << std::endl;
    if (_method == "POST" && _position != -1) {
        _headersSize = _headers.size() + _position;
        _contentLength = _headersSize;
        buffer.erase(0, _headersSize);
        _headers.clear();
    }
    _message.str("");
    _message.clear();
}

bool request::getRequestLine(void) {
    std::string tmp;
    int         pos;

    _statusCode = 200;
    std::getline(_message, tmp, '\n');
    for (int i = 0; i < 3; i++) {
        pos = tmp.find(' ');
        if (i == 0)
            _method = tmp.substr(0, pos);
        else if (i == 1)
            _URI = tmp.substr(0, pos);
        else
            _http = tmp.substr(0, tmp.size() == 0 ? 0 : (tmp.size() - 1));
        if (pos != -1)
            tmp.erase(0, (pos + 1));
    }
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        _statusCode = 405;
        return (false);
    }
    if (_URI.size() > 2048U) {
        _statusCode = 414;
        return (false);
    }
    if (_http.size() == 0 || _http != "HTTP/1.1") {
        _statusCode = 505;
        return (false);
    }
    return (true);
}

void request::getHeaders(void) {
    std::string tmp;
    int         size;
    int         pos;

    while (1) {
        std::getline(_message, tmp, '\n');
        std::cout << "headre: " << tmp << std::endl;
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
        if (_key == "Connection:")
            _connection = _value;
        else if (_key == "Transfer-Encoding:")
            _chunkedTransfer = true; // COULD BE A BUG SHOULD DOUBLE CHECK
        _msgrequest.insert( std::pair<std::string, std::string>(_key, _value));
    }
}


/*         POST METHOD PARSING REQUEST         */
void request::postBodyInfos(std::string &infos) {
    std::string tmp;
    int         size;
    int         pos;

    _message << infos;
    if (_chunkedTransfer)
        std::getline(_message, tmp, '\n');
    for (int i = 0; i < 3; i++) {
        std::getline(_message, tmp, '\n');
        size = tmp.size();
        _bodyInfoSize += size;
        if (i == 0){
            _boundary = tmp;
        }
        else if (i == 1) {
             if ((pos = tmp.find("filename=")) != -1)
                _filename = tmp.substr((pos + 10), (size - (pos + 12)));
        }
        else {
            pos = tmp.find(':');
            _contentType = tmp.substr((pos + 2), (size - pos));
        }
    }

    _message.str("");
    _message.clear();
}

void request::erasePostRequestHeaders(client &_client) {
    postBodyInfos(_client._requestBody);
    if (!_multipleFiles) {
        _headersSize = _bodyInfoSize + 5; // 5 for the size of boundary + content disposition + content-type + \n
        _client._requestBody.erase(0, _headersSize);
        _finaleBoundary = _boundary;
        _finaleBoundary.insert((_finaleBoundary.size() - 1), "--");
        _position = -1;
    }
    else {
        _position = _client._requestBody.find(_boundary);
        if (_position != -1) {
            _headersSize = _bodyInfoSize + 5;
            _client._requestBody.erase(0, _headersSize);
        }
    }
    _bodyInfoSize = 0;
}

void    request::postMethod(client &_client) {

    _position1 = _client._requestBody.find(_boundary);
    if (_position1 != -1) { // in case another boundary "body infos" means another body exists
        if(_chunkedTransfer)
            chunkedPostRequestBody(_client._requestBody, _client);
        else {
            normalPostRequestBody(_client._requestBody, _position1, _client);
        }
        _client._requestBody.erase(0, _position1);
        _multipleFiles = true;
        erasePostRequestHeaders(_client);
    }
    // FINALE STAPE
    _position2 = _client._requestBody.find(_finaleBoundary); // find the last boundary
    if (_position2 != -1) { // End of receiving
        while ((_position1 = _client._requestBody.find(_boundary)) != -1) {
            if(_chunkedTransfer)
                chunkedPostRequestBody(_client._requestBody, _client);
            else
                normalPostRequestBody(_client._requestBody, _position1, _client);
            _client._requestBody.erase(0, _position1);
            _multipleFiles = true;
            erasePostRequestHeaders(_client);
        }
        // SEND THE LAST _client._requestBody
        _position2 = _client._requestBody.find(_finaleBoundary); // update the postion in case we had lots of files
        if (_chunkedTransfer)
            chunkedPostRequestBody(_client._requestBody, _client);
        else
            normalPostRequestBody(_client._requestBody, _position2, _client);
        _client._requestBody.clear();
        _client.enableStartSend();
        _msgrequest.clear();
    }
}

void    request::normalPostRequestBody(std::string &buffer, int boundary_Position, client &_client) {
    std::string fullPath;

    fullPath.clear();
    _position = buffer.find(_finaleBoundary);
    if (_position == boundary_Position)
        buffer.erase(boundary_Position, (_boundary.size() + 4));
    _body.append(buffer, 0, boundary_Position);
    if (_body.size() <= (_client._clientBodySize * MEGA)) {
        fullPath = _client._uploadPath;
        fullPath.append("/");
        fullPath.append(_filename);
        std::cout << "This is fuul path =========> " << fullPath << std::endl;
        std::ofstream file(fullPath);
        if (file.is_open()) {
            file << _body;
            file.close();
        }
    }
    else
        _client._errorCode = 413;
    _body.clear();
}

void    request::chunkedPostRequestBody(std::string &buffer, client &_client) {
    std::string fullPath;
    _chunksVector = ft_split(buffer, "\r\n");
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
        _position = buffer.find(_chunkSizeVector[i]); // find size of the chunk in the body
        if (_position != -1) {
            _chunkSize = std::stoul(_chunkSizeVector[i], nullptr, 16);
            _chunks = buffer.substr((_position + _chunkSizeVector[i].size() + 2), _chunkSize);
            _body.append(_chunks);
            buffer.erase(_position, (_chunkSizeVector[i].size() + 2));
        }
    }
    if (_body.size() <= (_client._clientBodySize * MEGA))
    {
        fullPath = _client._uploadPath;
        fullPath.append("/");
        fullPath.append(_filename);
        std::ofstream file(fullPath);
        if (file.is_open()) {
            file << _body;
            file.close();
        }
    }
    _body.clear();
    _chunksVector.clear();
    _chunkSizeVector.clear();
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

/*         GET METHOD PARSING REQUEST         */





request::~request() {}
