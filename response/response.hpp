#pragma once
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"
#include "../client/client.hpp"

#define BUFFSIZE 16000
class response {

    private:
        std::string         _statusLine;
        std::string         _headers;
        std::string         _message;
        std::string         _contentType;
        std::string         _contentLength;
        std::string         _connexion;
        std::string         _type;
        std::string         _date;
        std::string         _serverName;
        std::string         _location;
        std::string         _body;
        std::string         _tmpBody;
        int                 _bytesCounter;
        int                 _bytesSend;

    public:
        response();
        std::string         _path;
        std::string         _locationContent;
        bool                _startSend;
        bool                _endSend;
        int code;

        void errorMessage(int code);
        void contentType(const std::string& file);
        std::string statusLine(void);
        std::string contentLengthHeader(int size);
        std::string contentTypeHeader(void);
        std::string connexionHeader(void);
        std::string serverNameHeader(client &_client);
        std::string dateHeader(void);
        std::string location(void);
        std::string getHeaders(int size, client &_client);
        std::string deleteHeaders(int size, client &_client);
        std::string postHeaders(int size, class client &_client);
        bool postMethodResponse(class client &_client);
        bool getMethodResponse(client &_client);
        bool deleteMethodResponse(client &_client);
        int get_file_size(void);
        std::string	readFile(void);
        void createHtmlFile(std::string fName);

        ~response();
};

#endif
