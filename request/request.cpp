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
    // for (it = _msgrequest.begin(); it != _msgrequest.end(); it++)
    //     std::cout << (*it).first << (*it).second << '\n';
    // std::cout << _msgrequest.size() << std::endl;
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
    // std::cout << _headers << std::endl;
}

// void request::get_body_info(void) {
//     std::string tmp;
//     int         size;
//     int         pos;

//     for (int i = 0; i < 3; i++) {
//         std::getline(_message, tmp, '\n');
//         size = tmp.size();
//         _body_info_size += size;
//         if (i == 0)
//             _boundary = tmp;
//         else if (i == 1) {
//              if ((pos = tmp.find("filename=")) != -1)
//                 _filename = tmp.substr((pos + 10), (size - (pos + 12)));
//         }
//         else {
//             pos = tmp.find(':');
//             _content_type = tmp.substr((pos + 2), (size - pos));
//         }
//     }
//     // std::cout << _boundary << '\n';
//     // std::cout << _filename << '\n';
//     // std::cout << _content_type << '\n';
// }

void request::get_body_info(void) {
    std::string tmp;
    int         size;
    int         pos;

    std::getline(_message, tmp, '\n');
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
    // std::cout << _boundary << '\n';
    // std::cout << _filename << '\n';
    // std::cout << _content_type << '\n';
}

request::~request() {}