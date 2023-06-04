#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../header.hpp"


class response {

    private:
        std::stringstream   ss;
        std::string         _message;
        std::string         _content_type;
        std::string         _type;
        std::string         _content_length;
        std::string         _date;
        std::string         _server_name;
        std::string         _httpv;

    public:
        response();
        std::string         _status_line;
        void response_generator(int coode);
        void GET_response(void);
        void POST_response(void);
        void DELETE_response(void);
        void content_length(void);
        std::string content_type(void);
        void connexion(void);
        std::string server_name(void);
        std::string date(void);
        void get_error_message(int code);
        void get_content_type(const std::string& file);
        ~response();
};

#endif