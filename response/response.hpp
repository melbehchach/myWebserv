#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"


class response {

    private:
        std::string     _status_line;
        std::string     _message;
        std::string     _content_type;
        std::string     _content_length;
        std::string     _connexion;
        std::string     _server_name;
        std::string     _httpv;
        std::stringstream   ss;
        int             _status_code;

    public:
        response();
        std::string response_generator(void);
        void GET_response(void);
        void POST_response(void);
        void DELETE_response(void);
        void error_message(int code);
        ~response();
};

#endif