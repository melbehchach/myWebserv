#include "./headers/ConfigValues.hpp"

ConfigValues::ConfigValues(std::string str) : _Root(false),
											  _Index(false),
											  _ClientMaxBodySize(false),
											  _ErrorPage(false),
											  _AutoIndex(false),
											  _ReturnValue(false),
											  _RootPath(str),
											  _ClientMaxSize(1),
											  _ErrorPages(),
											  _AutoIndexStatus(false),
											  _ReturnDir(ReturnDir())
{
	Index input("index.php index.html index.htm index.nginx-debian.html");
	_IndexList = input.GetIndex();
}

ConfigValues::~ConfigValues() {}

ConfigValues &ConfigValues::operator=(const ConfigValues &obj)
{
	if (this != &obj)
	{
		_Root = obj._Root;
		_Index = obj._Index;
		_ClientMaxBodySize = obj._ClientMaxBodySize;
		_ErrorPage = obj._ErrorPage;
		_AutoIndex = obj._AutoIndex;
		_ReturnValue = obj._ReturnValue;
		_RootPath = obj._RootPath;
		_IndexList = obj._IndexList;
		_ClientMaxSize = obj._ClientMaxSize;
		_ErrorPages = obj._ErrorPages;
		_AutoIndexStatus = obj._AutoIndexStatus;
		_ReturnDir = obj._ReturnDir;
	}
	return *this;
}

ConfigValues::ConfigValues(const ConfigValues &obj)
{
	*this = obj;
}

void ConfigValues::SetRoot(std::string val)
{
	_Root = true;
	_RootPath.clear();
	Root RootVal(val);
	_RootPath = val;
}

void ConfigValues::SetIndex(std::string val)
{
	_Index = true;
	_IndexList.clear();
	Index IndexVal(val);
	_IndexList = IndexVal.GetIndex();
}

void ConfigValues::SetCmbs(std::string val)
{
	_ClientMaxBodySize = true;
	ClientMaxBodySize cmbs(val);
	_ClientMaxSize = cmbs.GetVal();
}

void ConfigValues::SetErrorPage(std::string val)
{
	_ErrorPage = true;
	AddToErrorPageMap(&_ErrorPages, val);
}

void ConfigValues::SetAutoIndexDir(std::string val)
{
	_AutoIndex = true;
	_AutoIndexStatus = SetAutoIndex(val);
}

void ConfigValues::SetReturnDir(std::string val)
{
	_ReturnValue = true;
	ReturnDir var(val);
	_ReturnDir = var;
}

void ConfigValues::AddToErrorPageMap(std::map<int, std::string> *map, std::string input)
{
	int code;
	if (input.compare("") == 0)
		throw std::invalid_argument("Error: Missing Error Page Argument");
	std::vector<std::string> items = ToVectorOfStrings(input);
	// uri = protocol + file path
	std::string uri = items[items.size() - 1];
	if (uri[0] != '/')
		throw std::invalid_argument("Error: Invalid Error URI");
	for (size_t i = 0; i < (items.size() - 1); i++)
	{
		if (IsNumber(items[i]) == true)
		{
			code = std::atoi(items[i].c_str());
			if (code < 300 || code > 599)
				throw std::invalid_argument("Error: Bad Error Code");
			std::map<int, std::string>::iterator it = map->find(code);
			if (it == map->end())
				map->insert(std::pair<int, std::string>(code, uri));
			else
				throw std::invalid_argument("Error: Duplicate Error Code");
		}
		else
			throw std::invalid_argument("Error: Invalid Error Code");
	}
}

std::string ConfigValues::GetRoot() const
{
	return _RootPath;
}

std::vector<std::string> ConfigValues::GetIndex() const
{
	return _IndexList;
}
size_t ConfigValues::GetCmbs() const
{
	return _ClientMaxSize;
}
std::map<int, std::string> ConfigValues::GetErrorPage() const
{
	return _ErrorPages;
}
bool ConfigValues::GetAutoIndexDir() const
{
	return _AutoIndexStatus;
}
ReturnDir ConfigValues::GetReturn() const
{
	return _ReturnDir;
}
bool ConfigValues::HasErrorPage() const
{
	return _ErrorPage;
}
