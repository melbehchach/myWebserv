#ifndef PARSING_HPP
#define PARSING_HPP

#include "./ServerContext.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstddef> // size_t
#include <stdlib.h> // exit()

class ConfigFileParser {

	private:
		std::string _FileName;
		std::string _FileContent;
		std::vector<ServerContext> _servers;
		int _NumberOfServerContexts;
	public:
		ConfigFileParser();
		ConfigFileParser(const ConfigFileParser &obj);
		~ConfigFileParser();
		ConfigFileParser &operator=(const ConfigFileParser &obj);

		void ParseFile(int ac, char **av);
		void CheckArgs(int ac, char **av);
		bool CheckFile(char *FileName);
		void CheckBrackets();
		void CheckServerBlock();
		bool CheckServerValidity(std::string file, size_t *pos);
		void MoveToServerContext(size_t *pos);
		void SetServerContext(size_t *pos);

		std::string GetConfigFile() const;
		const std::vector<ServerContext> &GetServers() const;
		size_t GetNumberOfServers() const;

};


#endif

