#ifndef SERVERNAME_HPP
#define SERVERNAME_HPP

#include <string>
#include <vector>

class ServerName {
	private:
		std::string _SN;
		std::vector<std::string> _SNV;
		ServerName();
		ServerName(const ServerName &obj);
		ServerName &operator=(const ServerName& obj);
	public:
		ServerName(std::string name);
		void SplitServerName(std::string str);
		std::vector<std::string> GetServerNames()const ;
};


#endif
