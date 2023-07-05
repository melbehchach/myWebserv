#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../header.hpp"
#include "../response/response.hpp"

#define MAX_HEADER 8190

class request {

    private:
        

        std::stringstream                   _message;
        std::string                         _http;
        std::string                         _key;
        std::string                         _value;

        bool check_request_line(void);
        void get_headers(void);
        void get_body_info(void);

    public:
        request();
        std::multimap<std::string, std::string>             _msgrequest;
        std::multimap<std::string, std::string>::iterator   it;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _boundary;
        std::string                         _connexion;
        std::string                         _filename;
        std::string                         _content_type;
        std::string                         _body;
        std::string                         _headers;
        int                                 _content_length;
        int                                 _status_code;
        int                                 _body_info_size;

        ~request();
        void    get_request(std::string& headres);

};


#endif