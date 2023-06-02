#include "response.hpp"

response::response() {}

void response::response_generator(int coode) {
    _status_line.erase();
    error_message(coode);
    ss << coode;
    _httpv = "HTTP/1.1 ";
    _status_line.append(_httpv);
    _status_line.append(ss.str());
    _status_line.append(_message);
    _status_line.append("\r\n");
    ss.str("");
}

void response::GET_response(void) {

}

void response::POST_response(void) {

}

void response::DELETE_response(void) {
    
}

void response::error_message(int code) {
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
//     std::string content_type = "text/plain";
//     if (file_name.rfind('.') != std::string::npos) {
//         std::string ext =
//             file_name.substr(file_name.rfind('.'), file_name.size());
//         if (ext == ".html") {
//             content_type = "text/html";
//         } else if (ext == ".js") {
//             content_type = "application/javascript";
//         } else if (ext == ".css") {
//             content_type = "text/css";
//         } else if (ext == ".ico") {
//             content_type = "image/x-icon";
//         } else if (ext == ".jpeg" || ext == ".jpg") {
//             content_type = "image/jpeg";
//         } else if (ext == ".png") {
//             content_type = "image/png";
//         } else if (ext == ".mp4") {
//             content_type = "video/mp4";
//         } else if (ext == ".gif") {
//             content_type = "image/gif";
//         } else if (ext == ".tiff") {
//             content_type = "image/tiff";
//         } else if (ext == ".svg") {
//             content_type = "image/svg+xml";
//         } else if (ext == ".csv") {
//             content_type = "text/csv";
//         } else if (ext == ".xml") {
//             content_type = "application/xml";
//         } else if (ext == ".zip") {
//             content_type = "application/zip";
//         } else if (ext == ".mp3") {
//             content_type = "audio/mpeg";
//         } else if (ext == ".wma") {
//             content_type = "audio/x-ms-wma";
//         } else if (ext == ".ra") {
//             content_type = "audio/vnd.rn-realaudio";
//         } else if (ext == ".wav") {
//             content_type = "audio/x-wav";
//         } else if (ext == ".mpeg") {
//             content_type = "video/mpeg";
//         } else if (ext == ".mov") {
//             content_type = "video/quicktime";
//         } else if (ext == ".m4v") {
//             content_type = "video/x-m4v";
//         } else if (ext == ".qt") {
//             content_type = "video/quicktime";
//         } else if (ext == ".wmv") {
//             content_type = "video/x-ms-wmv";
//         } else if (ext == ".avi") {
//             content_type = "video/x-msvideo";
//         } else if (ext == ".flv") {
//             content_type = "video/x-flv";
//         } else if (ext == ".webm") {
//             content_type = "video/webm";
//         } else if (ext == ".odt") {
//             content_type = "application/vnd.oasis.opendocument.text";
//         } else if (ext == ".ods") {
//             content_type = "application/vnd.oasis.opendocument.spreadsheet";
//         } else if (ext == ".odp") {
//             content_type = "application/vnd.oasis.opendocument.presentation";
//         } else if (ext == ".odg") {
//             content_type = "application/vnd.oasis.opendocument.graphics";
//         } else if (ext == ".ods") {
//             content_type = "application/vnd.oasis.opendocument.spreadsheet";
//         } else if (ext == ".odt") {
//             content_type = "application/vnd.oasis.opendocument.text";
//         } else if (ext == ".ppt") {
//             content_type = "application/vnd.ms-powerpoint";
//         } else if (ext == ".pptx") {
//             content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.presentationml.presentation";
//         } else if (ext == ".xls") {
//             content_type = "application/vnd.ms-excel";
//         } else if (ext == ".xlsx") {
//             content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
//         } else if (ext == ".doc") {
//             content_type = "application/msword";
//         } else if (ext == ".docx") {
//             content_type =
//                 "application/"
//                 "vnd.openxmlformats-officedocument.wordprocessingml.document";
//         } else if (ext == ".xul") {
//             content_type = "application/vnd.mozilla.xul+xml";
//         }
//     }

//     return content_type;
// };