#include "header.hpp"
#include "./server/server.hpp"
#include "./parsing/headers/ConfigFileParser.hpp"

int main(int ac, char **av)
{
    try {
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
    } catch (std::exception const & e) {
        std::cout << e.what() << "\n";
        return -1;
    }


    return (0);
}
