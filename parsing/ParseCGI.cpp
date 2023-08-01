#include "./headers/ParseCGI.hpp"

ParseCGI::ParseCGI() {}

ParseCGI::ParseCGI(std::string str){
	if (str.compare("") == 0)
		throw std::invalid_argument("Error: Missing CGI Arguments");
	_Len = CountArgs(str);
	if (_Len > 2)
		throw std::invalid_argument("Error: Too Many CGI Arguments");
	else if (_Len < 1)
		throw std::invalid_argument("Error: Not Enough CGI Arguments");
	if (_Len == 1)
		OneArg(str);
	else {
		TwoArgs(str);
	}
}
ParseCGI::~ParseCGI() {}

ParseCGI::ParseCGI(const ParseCGI &obj) :
_Path(obj._Path),
_File(obj._File),
_Len(obj._Len) {}

ParseCGI &ParseCGI::operator=(const ParseCGI &obj) {
	if (this != &obj) {
		_Path = obj._Path;
		_File = obj._File;
		_Len = obj._Len;
	}
	return *this;
}

const std::string &ParseCGI::GetFileExtention() const {
	return _File;
}

const std::string &ParseCGI::GetFilePath() const {
	return _Path;
}

size_t ParseCGI::GetLen() const {
	return _Len;
}


void ParseCGI::OneArg(std::string str) {
	size_t start = str.find_first_not_of(" \n\r\v\t\f");
	size_t end = str.find_first_of(" \n\r\v\t\f", start);
	_Path = str.substr(start, end - start);
	if (IsPath(_Path) == false)
		throw std::invalid_argument("Error: Invalid CGI Path");
}

void ParseCGI::TwoArgs(std::string str) {
	size_t start = 0;
	size_t end = 0;
	size_t set = 0;

	for (size_t i = 0; i < str.size(); i++) {
		start = str.find_first_not_of(" \t\n\r\f\v", i);
		if (start == std::string::npos)
			break;
		end = str.find_first_of(" \n\t\r\f\v", start);
		if (start == end)
			break;
		if (set == 0) {
			SetFile(str.substr(start, end - start));
			set++;
		}
		else if (set == 1) {
			SetExecutableToArg(str.substr(start, end - start));
			set++;
			return ;
		}
		if (end == std::string::npos)
			break;
		i = end;
	}
}

void ParseCGI::SetFile(std::string str) {
	_File = str;
	if (_File[0] != '.')
		throw std::invalid_argument("Error: Invalid CGI File Extention");
}

void ParseCGI::SetExecutableToArg(std::string str) {
	_Path = str;
	if (IsPath(_Path) == false)
		throw std::invalid_argument("Error: Invalid CGI Path");
}

