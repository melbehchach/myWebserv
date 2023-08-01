#pragma once
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"
// #include "../request/request.hpp"
#include "../client/client.hpp"

#define BUFFSIZE 16000
class response {

    private:
                    /*  METHPDS */
        // client              _clientObj;
        std::string         _statusLine;
        std::string         _headers;
        std::string         _message;
        std::string         _contentType;
        std::string         _contentLength;
        std::string         _connexion;
        std::string         _type;
        std::string         _date;
        std::string         _serverName;
        std::string         _body;
        std::string         _tmpBody;
        int                 _bytesCounter;
        int                 _bytesSend;
                    /*  ATTRIBUTS   */

    public:
        response();
        std::string         _path;
        bool                _startSend;
        bool                _continueSend;
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
        std::string getHeaders(int size, client &_client);
        void postMethodResponse(class client &_client);
        bool getMethodResponse(client &_client);
        void deleteMethodResponse(client &_client);

        //  std::multimap<int, client> clientsToServ

        int get_file_size(void);
        std::string	readFile(void);
        // std::string	readFile(void);
        // void send_response(std::vector<struct pollfd>& fds, int index, std::string& body, std::string& path, int code);
        ~response();
};

#endif