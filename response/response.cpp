#include "response.hpp"

response::response()
{
}

std::string response::statusLine(void)
{
    errorMessage(code);
    _statusLine = "HTTP/1.1 ";
    _statusLine += std::to_string(code);
    _statusLine += _message;
    return (_statusLine);
}

std::string response::contentLengthHeader(int size)
{
    _contentLength = "Content-length: ";
    _contentLength += std::to_string(size);
    _contentLength += "\r\n";
    return (_contentLength);
}

std::string response::serverNameHeader(client &_client)
{
    _serverName = "server: ";
    _serverName += _client._hostname;
    _serverName += "\r\n";
    return (_serverName);
}

std::string response::dateHeader(void)
{
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

std::string response::contentTypeHeader(void)
{
    _contentType = "Content-type: ";
    _contentType += _type;
    _contentType += "\r\n";
    return (_contentType);
}

std::string response::location(void)
{
    _location = "Location: ";
    _location += _path;
    _location += "\r\n";
    return (_location);
}

/*                      DELETE METHOD RESPONSE                      */

std::string response::deleteHeaders(int size, client &_client) {
    _headers = statusLine();
    _headers += serverNameHeader(_client);
    _headers += dateHeader();
    if (code != 200 && code != 204)
    {
        contentType(_path);
        _headers += contentTypeHeader();
    }
    else
        size = 0;
    _headers += contentLengthHeader(size);
    _headers += "\r\n";
    return (_headers);
}

bool response::deleteMethodResponse(client &_client)
{
    _endSend = false;
    if (_client._startSend) {
        if (code != 200 && code != 204) {
            _path = "/Users/mel-behc/Desktop/myWebserv/cache/error.html";
            createHtmlFile(_path);
            _client._responseBody = deleteHeaders(get_file_size(), _client);
            _client._responseBody.append(readFile());
        }
        else {
            _client._responseBody = deleteHeaders(0, _client);
        }
        _client.disableStartSend();
    }
    if (_client._responseBody.size() > 0) {
        _bytesCounter = _client._responseBody.size() / 10;
        if (_client._responseBody.size() < BUFFSIZE)
            _bytesCounter = _client._responseBody.size();
        _bytesSend = send(_client._fd, _client._responseBody.c_str(), _bytesCounter, 0);
        if (_bytesSend < 0)
            std::cerr << strerror(errno) << '\n';
        _client._responseBody.erase(0, _bytesSend);
        _client._endSend = true;
    }
    else if (_client._responseBody.size() == 0) {
        if (_client._endSend) {
            _client._responseBody.clear();
            _endSend = true;
            _client._endSend = false;
            _client.resetAttributs();
        }
    }
    return (_endSend);
}

/*                      POST METHOD RESPONSE                      */
std::string response::postHeaders(int size, client &_client) {
    _headers = statusLine();
    _headers += serverNameHeader(_client);
    _headers += dateHeader();
    if (code != 200 && code != 204) {
        contentType(_path);
        _headers += contentTypeHeader();
    }
    else
        size = 0;
    _headers += contentLengthHeader(size);
    _headers += "\r\n";
    return (_headers);
}

bool response::postMethodResponse(class client &_client) {
    _endSend = false;
    if (_client._startSend) {
        if (code != 200 && code != 201) {
            _path = "/Users/mel-behc/Desktop/myWebserv/cache/error.html";
            createHtmlFile(_path);
            _client._responseBody = postHeaders(get_file_size(), _client);
            _client._responseBody.append(readFile());
        }
        else if (!_client._cgiOn) {
            _client._responseBody = postHeaders(0, _client);
        }
        _client.disableStartSend();
    }
    if (_client._responseBody.size() > 0) {
        _bytesCounter = _client._responseBody.size() / 10;
        if (_client._responseBody.size() < BUFFSIZE)
            _bytesCounter = _client._responseBody.size();
        _bytesSend = send(_client._fd, _client._responseBody.c_str(), _bytesCounter, 0);
        if (_bytesSend < 0)
            std::cerr << strerror(errno) << '\n';
        _client._responseBody.erase(0, _bytesSend);
        _client._endSend = true;
    }
    else if (_client._responseBody.size() == 0) {
        if (_client._endSend) {
            _client._responseBody.clear();
            _endSend = true;
            _client._endSend = false;
            _client.resetAttributs();
        }
    }
    return (_endSend);
}

/*                      GET METHOD RESPONSE                      */
std::string response::getHeaders(int size, client &_client)
{
    contentType(_path);
    _headers = statusLine();
    _headers += serverNameHeader(_client);
    _headers += dateHeader();
    _headers += contentTypeHeader();
    _headers += contentLengthHeader(size);
    if (code == 301)
    {
        _headers += location();
    }
    _headers += "\r\n";
    return (_headers);
}

bool response::getMethodResponse(client &_client)
{
    _endSend = false;
    if (_client._startSend) {
        if (code != 200 && code != 301) {
            if (!_client._errorPageExist) {
                _path = "/Users/mel-behc/Desktop/myWebserv/cache/error.html";
                createHtmlFile(_path);
            }
        }
        else if (_client._autoIndexOn && code != 301) {
            _path = "/Users/mel-behc/Desktop/myWebserv/cache/list.html";
            createHtmlFile(_path);
        }
        if (!_client._cgiOn) {
            _client._responseBody = getHeaders(get_file_size(), _client);
            _client._responseBody.append(readFile());
        }
        _client.disableStartSend();
    }
    if (_client._responseBody.size() > 0) {
        _bytesCounter = _client._responseBody.size() / 10;
        if (_client._responseBody.size() < BUFFSIZE)
            _bytesCounter = _client._responseBody.size();
        _bytesSend = send(_client._fd, _client._responseBody.c_str(), _bytesCounter, 0);
        if (_bytesSend < 0)
            std::cerr << strerror(errno) << '\n';
        _client._responseBody.erase(0, _bytesSend);
        _client._endSend = true;
    }
    if (_client._responseBody.size() == 0) {
        if (_client._endSend) {
            _endSend = true;
            _client._endSend = false;
            _client.resetAttributs();
        }
    }
    return (_endSend);
}

std::string response::readFile(void)
{ 
    std::ifstream file;
    std::string text;
    std::ostringstream streambuff;

    file.open(_path);
    if (file.is_open())
    {
        streambuff << file.rdbuf();
        text = streambuff.str();
    }
    file.close();
    return text;
}

int response::get_file_size(void)
{
    std::ifstream _file(_path, std::ios::in | std::ios::binary | std::ios::ate);
    int contentLength;

    if (!_file.is_open())
        contentLength = 0;
    else
    {
        std::streampos _fileSize = _file.tellg();
        contentLength = static_cast<int>(_fileSize);
    }
    _file.close();
    return (contentLength);
}

void response::createHtmlFile(std::string fName)
{
    std::ofstream file(fName);
    std::stringstream ss;
    std::string content;
    std::string errorCode;

    ss << code;
    ss >> errorCode;
    if (!file.is_open())
    {
        std::cerr << "Failed to create the file: " << fName << std::endl;
        return;
    }
    errorMessage(code);
    content = "<!DOCTYPE html>\n";
    content += "<html>\n";
    content += "<body>\n";
    if (code == 200 || code == 301)
        content += _locationContent;
    else
    {
        content += "<h1>Error page : ";
        content += errorCode;
        content += "</h1>\n";
        content += " <h2> ";
        content += _message.substr(0, (_message.size() - 2));
        content += " </h2>\n";
    }
    content += "</body>\n";
    content += "</html>\n";
    file << content;
    file.close();
}

void response::errorMessage(int code)
{
    if (code == 200)
        _message = " ok\r\n";
    else if (code == 204)
        _message = " No Content\r\n";
    else if (code == 400)
        _message = " Bad Request\r\n";
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
    else if (code == 301)
        _message = " Moved Permanently\r\n";
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
    else
    {
        code = 404;
        _message = " Not Found\r\n";
    }
}

void response::contentType(const std::string &_path)
{
    if (_path.rfind('.') != std::string::npos)
    {
        std::string ext = _path.substr(_path.rfind('.'), _path.size());
        if (ext == ".html")
            _type = "text/html";
        else if (ext == ".js")
            _type = "application/javascript";
        else if (ext == ".css")
            _type = "text/css";
        else if (ext == ".ico")
            _type = "image/x-icon";
        else if (ext == ".jpeg" || ext == ".jpg")
            _type = "image/jpeg";
        else if (ext == ".png")
            _type = "image/png";
        else if (ext == ".mp4")
            _type = "video/mp4";
        else if (ext == ".gif")
            _type = "image/gif";
        else if (ext == ".tiff")
            _type = "image/tiff";
        else if (ext == ".svg")
            _type = "image/svg+xml";
        else if (ext == ".csv")
            _type = "text/csv";
        else if (ext == ".xml")
            _type = "application/xml";
        else if (ext == ".zip")
            _type = "application/zip";
        else if (ext == ".mp3")
            _type = "audio/mpeg";
        else if (ext == ".wma")
            _type = "audio/x-ms-wma";
        else if (ext == ".ra")
            _type = "audio/vnd.rn-realaudio";
        else if (ext == ".wav")
            _type = "audio/x-wav";
        else if (ext == ".mpeg")
            _type = "video/mpeg";
        else if (ext == ".mov")
            _type = "video/quicktime";
        else if (ext == ".m4v")
            _type = "video/x-m4v";
        else if (ext == ".qt")
            _type = "video/quicktime";
        else if (ext == ".wmv")
            _type = "video/x-ms-wmv";
        else if (ext == ".avi")
            _type = "video/x-msvideo";
        else if (ext == ".flv")
            _type = "video/x-flv";
        else if (ext == ".webm")
            _type = "video/webm";
        else if (ext == ".odt")
            _type = "application/vnd.oasis.opendocument.text";
        else if (ext == ".ods")
            _type = "application/vnd.oasis.opendocument.spreadsheet";
        else if (ext == ".odp")
            _type = "application/vnd.oasis.opendocument.presentation";
        else if (ext == ".odg")
            _type = "application/vnd.oasis.opendocument.graphics";
        else if (ext == ".ods")
            _type = "application/vnd.oasis.opendocument.spreadsheet";
        else if (ext == ".odt")
            _type = "application/vnd.oasis.opendocument.text";
        else if (ext == ".ppt")
            _type = "application/vnd.ms-powerpoint";
        else if (ext == ".pptx")
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.presentationml.presentation";
        else if (ext == ".xls")
            _type = "application/vnd.ms-excel";
        else if (ext == ".xlsx")
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        else if (ext == ".doc")
            _type = "application/msword";
        else if (ext == ".docx")
            _type =
                "application/"
                "vnd.openxmlformats-officedocument.wordprocessingml.document";
        else if (ext == ".xul")
            _type = "application/vnd.mozilla.xul+xml";
        contentTypeHeader();
    }
}

response::~response() {}
