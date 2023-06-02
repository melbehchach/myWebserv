#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../header.hpp"
#include "../response/response.hpp"

#define MAX_HEADER 8190

class request {

    private:
        std::multimap<std::string, std::string>             _msgrequest;
        std::multimap<std::string, std::string>::iterator   it;

        std::stringstream                   _message;
        std::string                         _reqHeaders;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _http;
        std::string                         _key;
        std::string                         _value;
        response                            _httpResponse;


        
        void method_checker(void);
        void uri_checker(void);
        void httpv_checker(void);
        bool get_request_line(void);
        void get_headers(void);


    public:
        
        int                                 _status_code;
        request();
        ~request();
        void    get_request(std::string& headres);

};


#endif