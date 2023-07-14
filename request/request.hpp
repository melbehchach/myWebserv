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
        std::multimap<std::string, std::string>::iterator   _it;
        std::vector<std::string>            _chunksVector;
        std::vector<std::string>            _chunkSizeVector;
        // std::ofstream                       file;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _boundary;
        std::string                         _finaleBoundary;
        std::string                         _connexion;
        std::string                         _filename;
        std::string                         _content_type;
        std::string                         _body;
        std::string                         _headers;
        std::string                         _chunks;
        std::string                          _content_length;
        int                                 _status_code;
        int                                 _body_info_size;
        int                                 position;
        int                                 _chunkSize;
        bool                                _chunkedTransfer;
        int                                 headersSize;
        int                                 _nbrFiles;


        // POST REQUEST PARSER
            


        void                                get_request(std::string& headres);
        void                                _ParseRequestHeaders(std::string& tmpBody);
        void                                _parseChunkedRequestBody(std::string &tmpBody);
        void                                _parseNormalRequestBody(std::string &tmpBody, int boundaryPosition);
        std::vector<std::string>            ft_split(const std::string &str, const std::string &del);

        ~request();

};


#endif