#include "./headers/ServerName.hpp"


ServerName::ServerName(std::string name) : _SN(name){
	SplitServerName(name);
}

void ServerName::SplitServerName(std::string str) {
	std::string tmp = "";
	for (size_t i = 0; i < str.size(); i++) {
		if (!std::isspace(str[i])) {
			tmp += str[i];
			if (i == str.size() - 1) {
				_SNV.push_back(tmp);
				tmp.erase();
			}
		}
		else if (tmp.size() != 0) {
			_SNV.push_back(tmp);
			tmp.erase();
		}
	}
}

std::vector<std::string> ServerName::GetServerNames()const {
	return _SNV;
}
