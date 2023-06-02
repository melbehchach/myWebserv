#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"


class response {

    private:
        std::stringstream   ss;
        std::string         _message;
        std::string         _content_type;
        std::string         _content_length;
        std::string         _connexion;
        std::string         _server_name;
        std::string         _httpv;

    public:
        response();
        std::string         _status_line;
        void response_generator(int coode);
        void GET_response(void);
        void POST_response(void);
        void DELETE_response(void);
        void error_message(int code);
        ~response();
};

#endif