#include "response.hpp"

response::response() {
    _body = "";
}



// std::string	response::readFile(void) { // RETURN THE CONTENT OF A FILE AS A STD::STRING
// 	std::ifstream 	file;
// 	std::string text;
// 	std::ostringstream streambuff;

// 	file.open();
// 	if (file.is_open()) {
// 		streambuff << file.rdbuf();
// 		text = streambuff.str();
// 	}
// 	file.close();
// 	return text;
// }

// void response::GET_response(std::vector<struct pollfd>& fds, int index, std::string& body, const std::string& filename, int code) {
    // if (body.size() == 0) {
    //     _path = filename;
    //     get_content_type(filename);
    //     body = headers_generator(code);
    //     body += readFile();
    // }
    // if (body.size() > 0) {
    //     bytToSend = body.size() / 10;
    //     if (body.size() < BUFFSIZE)
    //         bytToSend = body.size();
    //     retsend = send(fds[index].fd, body.c_str(), bytToSend, 0);
    //     if (retsend < 0)
    //         std::cout << strerror(errno) << '\n';
    //     body.erase(0, retsend);
    // }
    // else {
    //     close(fds[index].fd);
    //     fds.erase(fds.begin() + index);
    //     body.erase();
    // }
// }

std::string response::getHeaders(int code) {
    _status_line.erase();
    errorMessage(code);
    ss << code;
    _httpv = "HTTP/1.1 ";
    _status_line.append(_httpv);
    _status_line.append(ss.str());
    _status_line.append(_message);
    _headers = _status_line;
    _headers += date();
    _headers += server_name();
    _headers += _content_type;
    _headers += _content_length;
    _headers += "\r\n";
    ss.str("");
    ss.clear();
    _headers.clear();
    return (_headers);
}

// std::string response::getMethod(int code) {
    // if (_body.size() == 0) { // need to handl the prblem of the clients vector
        // content_length(get_file_size());
        // _body = getHeaders(code);
    // }
    // if (_body.size() > 0) {
    //     bytToSend = _body.size() / 10;
    //     if (_body.size() < BUFFSIZE)
    //         bytToSend = _body.size();
    //     retsend = send(fds[index].fd, _body.c_str(), bytToSend, 0);
    //     if (retsend < 0)
    //         std::cout << strerror(errno) << '\n';
    //     _body.erase(0, retsend);
    // }
    // else {
    //     if (connexion == "keep-alive") {
            
    //     }
    //     close(fds[index].fd);
    //     fds.erase(fds.begin() + index);
    //     _body.erase();
    // }
//     return (_body);
// }

std::string response::postMethod(int code) {
    _status_line.erase();
    errorMessage(code);
    ss << code;
    _httpv = "HTTP/1.1 ";
    _status_line.append(_httpv);
    _status_line.append(ss.str());
    _status_line.append(_message);
    _headers = _status_line;
    _headers += date();
    _headers += server_name();
    _headers += "\r\n";
    ss.str("");
    ss.clear();
    return (_headers);
}

void response::DELETE_response(void) {
    
}

std::string response::content_length(int size) {
    _content_length = "Content-length: ";
    _content_length += std::to_string(size);
    _content_length += "\r\n";
    return (_content_length);
}

void response::content_type(void) {
    _content_type = "Content-type: ";
    _content_type += _type;
    _content_type += "; charset=UTF-8";
    _content_type += "\r\n";
}

void response::connexion(void) {

}

std::string response::server_name(void) {
    _server_name = "server: ";
    _server_name += "Aba7law's webserv v1.0";
    _server_name += "\r\n";
    return (_server_name);
}

std::string response::date(void) {
    // Get the current system time
    std::time_t currentTime = std::time(NULL);
    // Convert the time_t object to a string
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%a, %d %b %Y %H:%M:%S %Z", std::localtime(&currentTime));
    _date = "Date: ";
    _date += timeString;
    _date += " GMT";
    _date += "\r\n";
    return (_date);
}


// void response::send_response(std::vector<struct pollfd>& fds, int index, std::string& body, const std::string& filename, int code) {

// }


std::string	response::readFile(void) { // RETURN THE CONTENT OF A FILE AS A STD::STRING
	std::ifstream 	file;
	std::string     text;
	std::ostringstream streambuff;
	file.open(_path);
	if (file.is_open()) {
		streambuff << file.rdbuf();
		text = streambuff.str();
	}
	file.close();
	return text;
}

int response::get_file_size(void) {
    std::ifstream _file(_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!_file.is_open())
        std::cout << "error kabiiiiiir \n";
    std::streampos _fileSize = _file.tellg();
    int contentLength = static_cast<int>(_fileSize);
    _file.close();
    return (contentLength);
}


void response::errorMessage(int code) {
    if (code == 200)
        _message = " ok\r\n";
    else if (code == 400) 
        _message = " Bad Reques\r\n";
    else if (code == 401) 
        _message = " Unauthorized\r\n";
    else if (code == 402) 
        _message = " Payment Required\r\n";
    else if (code == 404) 
        _message = " Not Found\r\n";
    else if (code == 405) 
        _message = " Method Not Allowed\r\n";
    else if (code == 403) 
        _message = " Forbidden\r\n";
    else if (code == 406) 
        _message = " Not Acceptable\r\n";
    else if (code == 407) 
        _message = " Proxy Authentication Required\r\n";
    else if (code == 408) 
        _message = " Request Timeout\r\n";
    else if (code == 409) 
        _message = " Conflict\r\n";
    else if (code == 410) 
        _message = " Gone\r\n";
    else if (code == 411) 
        _message = " Length Required\r\n";
    else if (code == 412) 
        _message = " Precondition Failed\r\n";
    else if (code == 413) 
        _message = " Payload Too Large\r\n";
    else if (code == 414) 
        _message = " URI Too Long\r\n";
    else if (code == 415) 
        _message = " Unsupported Media Type\r\n";
    else if (code == 416) 
        _message = " Range Not Satisfiable\r\n";
    else if (code == 417) 
        _message = " Expectation Failed\r\n";
    else if (code == 422) 
        _message = " Unprocessable Entity\r\n";
    else if (code == 423) 
        _message = " Locked\r\n";
    else if (code == 424) 
        _message = " Failed Dependency\r\n";
    else if (code == 425) 
        _message = " Too Early\r\n";
    else if (code == 426) 
        _message = " Upgrade Required\r\n";
    else if (code == 428) 
        _message = " Precondition Required\r\n";
    else if (code == 429) 
        _message = " Too Many Requests\r\n";
    else if (code == 431) 
        _message = " Request Header Fields Too Large\r\n";
    else if (code == 451) 
        _message = " Unavailable For Legal Reasons\r\n";
    else if (code == 500) 
        _message = " Internal Server Error\r\n";
    else if (code == 501) 
        _message = " Not Implemented\r\n";
    else if (code == 502) 
        _message = " Bad Gateway\r\n";
    else if (code == 503) 
        _message = " Service Unavailable\r\n";
    else if (code == 504) 
        _message = " Gateway Timeout\r\n";
    else if (code == 505) 
        _message = " HTTP Version Not Supported\r\n";
    else if (code == 506) 
        _message = " Variant Also Negotiates\r\n";
    else if (code == 507) 
        _message = " Insufficient Storage\r\n";
    else if (code == 508) 
        _message = " Loop Detected\r\n";
    else if (code == 510) 
        _message = " Not Extended\r\n";
    else if (code == 511) 
        _message = " Network Authentication Required\r\n";
    else {
        code = 404;
        _message = " Not Found\r\n";
    }
}

void response::contentType(const std::string& file)
{
    if (file.rfind('.') != std::string::npos) {
        std::string ext =
            file.substr(file.rfind('.'), file.size());
        if (ext == ".html") {
            _type = "text/html";
        } 
        else if (ext == ".js") {
            _type = "application/javascript";
        } 
        else if (ext == ".css") {
            _type = "text/css";
        } 
        else if (ext == ".ico") {
            _type = "image/x-icon";
        } 
        else if (ext == ".jpeg" || ext == ".jpg") {
            _type = "image/jpeg";
        }
        else if (ext == ".png") {
            _type = "image/png";
        }
        else if (ext == ".mp4") {
            _type = "video/mp4";
        }
        else if (ext == ".gif") {
            _type = "image/gif";
        }
        else if (ext == ".tiff") {
            _type = "image/tiff";
        }
        else if (ext == ".svg") {
            _type = "image/svg+xml";
        }
        else if (ext == ".csv") {
            _type = "text/csv";
        }
        else if (ext == ".xml") {
            _type = "application/xml";
        }
        else if (ext == ".zip") {
            _type = "application/zip";
        }
        else if (ext == ".mp3") {
            _type = "audio/mpeg";
        }
        else if (ext == ".wma") {
            _type = "audio/x-ms-wma";
        }
        else if (ext == ".ra") {
            _type = "audio/vnd.rn-realaudio";
        }
        else if (ext == ".wav") {
            _type = "audio/x-wav";
        }
        else if (ext == ".mpeg") {
            _type = "video/mpeg";
        }
        else if (ext == ".mov") {
            _type = "video/quicktime";
        }
        else if (ext == ".m4v") {
            _type = "video/x-m4v";
        }
        else if (ext == ".qt") {
            _type = "video/quicktime";
        }
        else if (ext == ".wmv") {
            _type = "video/x-ms-wmv";
        }
        else if (ext == ".avi") {
            _type = "video/x-msvideo";
        }
        else if (ext == ".flv") {
            _type = "video/x-flv";
        }
        else if (ext == ".webm") {
            _type = "video/webm";
        }
        else if (ext == ".odt") {
            _type = "application/vnd.oasis.opendocument.text";
        }
        else if (ext == ".ods") {
            _type = "application/vnd.oasis.opendocument.spreadsheet";
        }
        else if (ext == ".odp") {
            _type = "application/vnd.oasis.opendocument.presentation";
        }
        else if (ext == ".odg") {
            _type = "application/vnd.oasis.opendocument.graphics";
        }
        else if (ext == ".ods") {
            _type = "application/vnd.oasis.opendocument.spreadsheet";
        }
        else if (ext == ".odt") {
            _type = "application/vnd.oasis.opendocument.text";
        }
        else if (ext == ".ppt") {
            _type = "application/vnd.ms-powerpoint";
        }
        else if (ext == ".pptx") {
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.presentationml.presentation";
        }
        else if (ext == ".xls") {
            _type = "application/vnd.ms-excel";
        }
        else if (ext == ".xlsx") {
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        }
        else if (ext == ".doc") {
            _type = "application/msword";
        }
        else if (ext == ".docx") {
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.wordprocessingml.document";
        }
        else if (ext == ".xul") {
            _type = "application/vnd.mozilla.xul+xml";
        }
    }
    content_type();
}


response::~response() {}


// STATUS CODE
// HttpResponse HttpResponse::error_response(int status, const std::string& path) {
//     std::string message;
//     if (code == 400) {
//         message = "Bad Request";
//     } else if (code == 401) {
//         message = "Unauthorized";
//     } else if (code == 402) {
//         message = "Payment Required";
//     } else if (code == 404) {
//         message = "Not Found";
//     } else if (code == 405) {
//         message = "Method Not Allowed";
//     } else if (code == 403) {
//         message = "Forbidden";
//     } else if (code == 406) {
//         message = "Not Acceptable";
//     } else if (code == 407) {
//         message = "Proxy Authentication Required";
//     } else if (code == 408) {
//         message = "Request Timeout";
//     } else if (code == 409) {
//         message = "Conflict";
//     } else if (code == 410) {
//         message = "Gone";
//     } else if (code == 411) {
//         message = "Length Required";
//     } else if (code == 412) {
//         message = "Precondition Failed";
//     } else if (code == 413) {
//         message = "Payload Too Large";
//     } else if (code == 414) {
//         message = "URI Too Long";
//     } else if (code == 415) {
//         message = "Unsupported Media Type";
//     } else if (code == 416) {
//         message = "Range Not Satisfiable";
//     } else if (code == 417) {
//         message = "Expectation Failed";
//     } else if (code == 422) {
//         message = "Unprocessable Entity";
//     } else if (code == 423) {
//         message = "Locked";
//     } else if (code == 424) {
//         message = "Failed Dependency";
//     } else if (code == 425) {
//         message = "Too Early";
//     } else if (code == 426) {
//         message = "Upgrade Required";
//     } else if (code == 428) {
//         message = "Precondition Required";
//     } else if (code == 429) {
//         message = "Too Many Requests";
//     } else if (code == 431) {
//         message = "Request Header Fields Too Large";
//     } else if (code == 451) {
//         message = "Unavailable For Legal Reasons";
//     } else if (code == 500) {
//         message = "Internal Server Error";
//     } else if (code == 501) {
//         message = "Not Implemented";
//     } else if (code == 502) {
//         message = "Bad Gateway";
//     } else if (code == 503) {
//         message = "Service Unavailable";
//     } else if (code == 504) {
//         message = "Gateway Timeout";
//     } else if (code == 505) {
//         message = "HTTP Version Not Supported";
//     } else if (code == 506) {
//         message = "Variant Also Negotiates";
//     } else if (code == 507) {
//         message = "Insufficient Storage";
//     } else if (code == 508) {
//         message = "Loop Detected";
//     } else if (code == 510) {
//         message = "Not Extended";
//     } else if (code == 511) {
//         message = "Network Authentication Required";
//     } else {
//         code = 404;
//         message = "Not Found";
//     }
//     if (!path.empty() && is_file(path) && is_file_exists(path) &&
//         is_file_readable(path)) {
//         // if (TODO)
//         // cache ::
//         std::string to_serve;
//         if (files_cache.find(path) == files_cache.end()) {
//             std::ifstream file(path);
//             if (!file.is_open() || file.fail()) {
//                 return HttpResponse::error_response(500, "");
//             }
//             HttpResponse::files_cache[path] = to_serve =
//                 tools::open_to_serve(file);
//         } else {
//             to_serve = HttpResponse::files_cache[path];
//         }
//         // :: cache
//         return HttpResponse(status, "1.1", action)
//             .add_to_body(to_serve)
//             .add_content_type(path);
//     }

//     return HttpResponse(status, "1.1", action)
//         .add_to_body(generateErrorPage(status, action))
//         .add_content_type(".html");
// };



// CONTENT TYPE 
// std::string HttpResponse::get_content_type(const std::string& file_name) {
//     std::string _content_type = "text/plain";
//     if (file_name.rfind('.') != std::string::npos) {
//         std::string ext =
//             file_name.substr(file_name.rfind('.'), file_name.size());
//         if (ext == ".html") {
//             _content_type = "text/html";
//         } else if (ext == ".js") {
//             _content_type = "application/javascript";
//         } else if (ext == ".css") {
//             _content_type = "text/css";
//         } else if (ext == ".ico") {
//             _content_type = "image/x-icon";
//         } else if (ext == ".jpeg" || ext == ".jpg") {
//             _content_type = "image/jpeg";
//         } else if (ext == ".png") {
//             _content_type = "image/png";
//         } else if (ext == ".mp4") {
//             _content_type = "video/mp4";
//         } else if (ext == ".gif") {
//             _content_type = "image/gif";
//         } else if (ext == ".tiff") {
//             _content_type = "image/tiff";
//         } else if (ext == ".svg") {
//             _content_type = "image/svg+xml";
//         } else if (ext == ".csv") {
//             _content_type = "text/csv";
//         } else if (ext == ".xml") {
//             _content_type = "application/xml";
//         } else if (ext == ".zip") {
//             _content_type = "application/zip";
//         } else if (ext == ".mp3") {
//             _content_type = "audio/mpeg";
//         } else if (ext == ".wma") {
//             _content_type = "audio/x-ms-wma";
//         } else if (ext == ".ra") {
//             _content_type = "audio/vnd.rn-realaudio";
//         } else if (ext == ".wav") {
//             _content_type = "audio/x-wav";
//         } else if (ext == ".mpeg") {
//             _content_type = "video/mpeg";
//         } else if (ext == ".mov") {
//             _content_type = "video/quicktime";
//         } else if (ext == ".m4v") {
//             _content_type = "video/x-m4v";
//         } else if (ext == ".qt") {
//             _content_type = "video/quicktime";
//         } else if (ext == ".wmv") {
//             _content_type = "video/x-ms-wmv";
//         } else if (ext == ".avi") {
//             _content_type = "video/x-msvideo";
//         } else if (ext == ".flv") {
//             _content_type = "video/x-flv";
//         } else if (ext == ".webm") {
//             _content_type = "video/webm";
//         } else if (ext == ".odt") {
//             _content_type = "application/vnd.oasis.opendocument.text";
//         } else if (ext == ".ods") {
//             _content_type = "application/vnd.oasis.opendocument.spreadsheet";
//         } else if (ext == ".odp") {
//             _content_type = "application/vnd.oasis.opendocument.presentation";
//         } else if (ext == ".odg") {
//             _content_type = "application/vnd.oasis.opendocument.graphics";
//         } else if (ext == ".ods") {
//             _content_type = "application/vnd.oasis.opendocument.spreadsheet";
//         } else if (ext == ".odt") {
//             _content_type = "application/vnd.oasis.opendocument.text";
//         } else if (ext == ".ppt") {
//             _content_type = "application/vnd.ms-powerpoint";
//         } else if (ext == ".pptx") {
//             _content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.presentationml.presentation";
//         } else if (ext == ".xls") {
//             _content_type = "application/vnd.ms-excel";
//         } else if (ext == ".xlsx") {
//             _content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
//         } else if (ext == ".doc") {
//             _content_type = "application/msword";
//         } else if (ext == ".docx") {
//             _content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.wordprocessingml.document";
//         } else if (ext == ".xul") {
//             _content_type = "application/vnd.mozilla.xul+xml";
//         }
//     }

//     return _content_type;
// };