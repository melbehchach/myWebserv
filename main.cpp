#include "header.hpp"
#include "./server/server.hpp"
#include "./parsing/headers/ConfigFileParser.hpp"

using namespace std;

int main(int ac, char **av)
{
    ConfigFileParser configParser;
    configParser.ParseFile(ac, av);
    std::multimap<std::string, std::pair<string, string> > serverMap;
    for (size_t i = 0; i < configParser.GetNumberOfServers(); i++) {
    	std::multimap<std::string, std::string> tmp = configParser.GetServers()[i].GetListen();
    	for(std::multimap<std::string, std::string>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
    		serverMap.insert(std::make_pair(it->first, std::make_pair(it->second, configParser.GetServers()[i].GetServerNames()[0])));
    	}
    }
    // std::multimap<std::string, std::pair<std::string, std::string> > m;
    // m.insert(std::pair<std::string, std::pair<std::string, std::string> >("8080", std::make_pair("127.0.0.1", "localhost")));
    // m.insert(std::pair<std::string, std::pair<std::string, std::string> >("9090", std::make_pair("127.0.0.1", "localhost")));
    // m.insert(std::pair<std::string, std::pair<std::string, std::string> >("7070", std::make_pair("127.0.0.1", "localhost")));
    server serv(serverMap);


    return (0);
}
