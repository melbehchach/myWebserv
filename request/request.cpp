#include "request.hpp"

request::request() {
    _multipleFiles = false;
    _chunkedTransfer = false;
}

void request::requestHeader(std::string& buffer) { // SAME FUNCTION FOR POST GET AND DELETE
    _message << buffer;
    getRequestLine();
    getHeaders();
    position = buffer.find(_headers);
    if (_method == "POST" && position != -1) {
        headersSize = _headers.size() + position;
        _content_length = headersSize;
        buffer.erase(0, headersSize);
        _headers.erase();
    }
    _message.str("");
    _message.clear();
}

bool request::getRequestLine(void) { // reading the request line to get infos about http method uri and http version
    std::string tmp;
    int         pos;

    _status_code = 200;
    std::getline(_message, tmp, '\n');
    for (int i = 0; i < 3; i++) {
        pos = tmp.find(' ');
        if (i == 0)
            _method = tmp.substr(0, pos);
        else if (i == 1)
            _uri = tmp.substr(0, pos);
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
        std::cout << "checked" << std::endl;
        _status_code = 403;
        return (false);
    }
    if (_http != "HTTP/1.1") {
        _status_code = 505;
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
            _connexion = _value;
        else if (_key == "Transfer-Encoding:")
            _chunkedTransfer = true;
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
        std::getline(_message, tmp, '\n'); // skip the line containing the size of chunk
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
        // if (_boundary.size() == 0) IN CASE OF REQUEST DON'T HAVE ANY BODY INFOS
        //     break;
    }
    _message.str("");
    _message.clear();
}

void request::erasePostRequestHeaders(std::string& buffer) {
    postBodyInfos(buffer);
    if (!_multipleFiles) {
        headersSize = _body_info_size + 5; // 5 for the size of boundary + content disposition + content-type + \n
        buffer.erase(0, headersSize);
        _finaleBoundary = _boundary;
        _finaleBoundary.insert((_finaleBoundary.size() - 1), "--");
        position = -1;
    }
    else {
        position = buffer.find(_boundary);
        if (position != -1) {
            headersSize = _body_info_size + 5;
            buffer.erase(0, headersSize);
        }
    }
    _body_info_size = 0;
}

void    request::postMethod(std::string &data) {
    position1 = data.find(_boundary);
    if (position1 != -1) { // in case i find another boundary "body infos" means another body exists
        if(_chunkedTransfer)
            chunkedPostRequestBody(data);
        else
            normalPostRequestBody(data, position1);
        data.erase(0, position1);
        _multipleFiles = true;
        erasePostRequestHeaders(data);
    }
    // FINALE STAPE
    position2 = data.find(_finaleBoundary); // find the last boundary
    if (position2  != -1) { // End of receiving
        while ((position1 = data.find(_boundary)) != -1) {
            if(_chunkedTransfer)
                chunkedPostRequestBody(data);
            else
                normalPostRequestBody(data, position1);
            data.erase(0, position1);
            _multipleFiles = true;
            erasePostRequestHeaders(data);
        }
        // SEND THE LAST DATA
        position2 = data.find(_finaleBoundary); // update the postion in case we had lots of files
        if (_chunkedTransfer)
            chunkedPostRequestBody(data);
        else 
            normalPostRequestBody(data, position2);
        data.clear();
        _msgrequest.clear();
    }
}

void    request::normalPostRequestBody(std::string &buffer, int boundaryPosition) {
    position = buffer.find(_finaleBoundary);
    if (position == boundaryPosition)
        buffer.erase(boundaryPosition, (_boundary.size() + 4));
    _body.append(buffer, 0, boundaryPosition);
    std::ofstream file(_filename);
    if (file.is_open()) {
        file << _body;
        file.close();
    }
    _body.clear();
}

void    request::chunkedPostRequestBody(std::string &buffer) {  
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
        position = buffer.find(_chunkSizeVector[i]); // find size of the chunk in the body
        if (position != -1) {
            _chunkSize = std::stoul(_chunkSizeVector[i], nullptr, 16);
            _chunks = buffer.substr((position + _chunkSizeVector[i].size() + 2), _chunkSize);
            _body.append(_chunks);
            buffer.erase(position, (_chunkSizeVector[i].size() + 2));
        }
    }
    std::ofstream file(_filename);
    if (file.is_open()) {
        file << _body;
        file.close();
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