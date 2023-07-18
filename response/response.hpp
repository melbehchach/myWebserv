#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"
#include "../request/request.hpp"
#define BUFFSIZE 16000

class response {

    private:
                    /*  METHPDS */

        std::string         _status_line;
        std::string         _headers;
        std::string         _message;
        std::string         _content_type;
        std::string         _content_length;
        std::string         _connexion;
        std::string         _type;
        std::string         _date;
        std::string         _server_name;
        std::string         _body;
        int                 bytesCounter;
        int                 bytesSend;

                    /*  ATTRIBUTS   */

    public:
        response();
        std::string         _path;
        bool                startSend;
        bool                endSend;
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