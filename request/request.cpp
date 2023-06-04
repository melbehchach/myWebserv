#include "request.hpp"

request::request() {
}

void request::get_request(std::string& headers) {
    _message << headers;
    get_request_line();
    method_checker();
    uri_checker();
    httpv_checker();
    _message << headers;
    get_headers();
}

bool request::get_request_line(void) {
    _status_code = 200;
    for (int i = 0; i < 3; i++) {
        if (i == 0)
            std::getline(_message, _method, ' ');
        else if (i == 1)
            std::getline(_message, _uri, ' ');
        else {
            std::getline(_message, _http, ' ');
            _http = _http.substr(0, 8);
        }
    }
    _message.str("");
    _message.clear();
    return (true);
}

void request::method_checker(void) {
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        _status_code = 405;
    }
}

void request::uri_checker(void) {
    // std::cout << _uri << '\n';
    if (_uri.size() > 2048U) {
        _status_code = 414;
    }
    if (access(_uri.c_str(), F_OK | R_OK) != 0) {
        _status_code = 403;
    }
}

void request::httpv_checker(void) {
    if (_http != "HTTP/1.1") {
        _status_code = 505;
    }
}

void request::get_headers(void)
{
    std::string tmp;
    size_t pos;
    size_t size;

    std::getline(_message, tmp, '\n');
    while (std::getline(_message, tmp, '\n'))
    {
        if (tmp.size() < 2)
            break;
        size = tmp.size();
        pos = tmp.find(':');
        _key = tmp.substr(0, pos);
        _value = tmp.substr(pos, (size - pos));
        _msgrequest.insert( std::pair<std::string, std::string>(_key, _value) );
    }
    _message.str("");
    _message.clear();
    // for (it = _msgrequest.begin(); it != _msgrequest.end(); it++)
    //     std::cout << (*it).first << (*it).second << '\n';
    // _msgrequest.clear();
}

request::~request() {}