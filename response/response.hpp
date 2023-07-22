#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#define BUFFSIZE 16000

class response {

    private:
                    /*  METHPDS */

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
        bool                _endSend;
        int code;
        
        void errorMessage(int code);
        void contentType(const std::string& file);
        std::string statusLine(void);
        std::string contentLengthHeader(int size);
        std::string contentTypeHeader(void);
        std::string connexionHeader(void);
        std::string serverNameHeader(void);
        std::string dateHeader(void);
        std::string getHeaders(int size);
        void postMethodResponse(int fd);
        bool getMethodResponse(int fd);



        void DELETE_response(void);
        int get_file_size(void);
        std::string	readFile(void);
        // std::string	readFile(void);
        // void send_response(std::vector<struct pollfd>& fds, int index, std::string& body, std::string& path, int code);
        ~response();
};

#endif