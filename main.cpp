#include "header.hpp"
#include "./server/server.hpp"
#include "./parsing/headers/ConfigFileParser.hpp"

// using namespace std;

int main(int ac, char **av)
{
    // signal(SIG_IGN, SIGPIPE);
    ConfigFileParser configParser;
    configParser.ParseFile(ac, av);
    std::multimap<std::string, std::pair<std::string, std::string> > serverMap;
    for (size_t i = 0; i < configParser.GetNumberOfServers(); i++) {
    	std::multimap<std::string, std::string> tmp = configParser.GetServers()[i].GetListen();
    	for(std::multimap<std::string, std::string>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
    		serverMap.insert(std::make_pair(it->first, std::make_pair(it->second, configParser.GetServers()[i].GetServerNames()[0])));
    	}
    }

    server serv(serverMap, configParser);


    return (0);
}
