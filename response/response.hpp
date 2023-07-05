#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"
#include "../request/request.hpp"
// #define BUFFSIZE 1024

class response {

    private:
        std::stringstream   ss;
        std::string         _headers;
        std::string         _message;
        std::string         _content_type;
        std::string         _type;
        std::string         _date;
        std::string         _server_name;
        std::string         _httpv;
        std::string         _path;
        // request             _request;
        // size_t              bytToSend;
        // int                 retsend;
        // int                 counter;
        // request             requestObj;


    public:
        response();
        std::string         _status_line;
        std::string headers_generator(int coode);
        // int                 _content_length;
        // void GET_response(std::vector<struct pollfd>& fds, int index, std::string& body, const std::string& path, int code);
        void POST_response(void);
        void DELETE_response(void);
        void content_length(void);
        std::string content_type(void);
        void connexion(void);
        std::string server_name(void);
        std::string date(void);
        void get_error_message(int code);
        void get_content_type(const std::string& file);
        // std::string	readFile(void);
        // void send_response(std::vector<struct pollfd>& fds, int index, std::string& body, std::string& path, int code);
        ~response();
};

#endif