#include "header.hpp"
#include "./server/server.hpp"

int main()
{
    std::multimap<std::string, std::pair<std::string, std::string> > m;
    m.insert(std::pair<std::string, std::pair<std::string, std::string> >("8080", std::make_pair("127.0.0.1", "localhost")));
    m.insert(std::pair<std::string, std::pair<std::string, std::string> >("9090", std::make_pair("127.0.0.1", "localhost")));
    m.insert(std::pair<std::string, std::pair<std::string, std::string> >("7070", std::make_pair("127.0.0.1", "localhost")));   
    server serv(m);


    return (0);
}