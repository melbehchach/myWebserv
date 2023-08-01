#include "./headers/LocationContext.hpp"

LocationContext::LocationContext(std::string root) : ConfigValues(root),
													 _Uri(false),
													 _Cgi(false),
													 _AllowedMethods(false),
													 _Alias(false),
													 _LocationUri(LocationUri()),
													 _CGI_PASS(ParseCGI()),
													 _Methods(CheckMethods()) {}

LocationContext::LocationContext(std::string str, std::string root) : ConfigValues(root),
																	  _Uri(false),
																	  _Cgi(false),
																	  _AllowedMethods(false),
																	  _Alias(false),
																	  _LocationUri(LocationUri()),
																	  _CGI_PASS(ParseCGI()),
																	  _Methods(CheckMethods())
{
	GetValueDirectivePaires(str);
}

LocationContext &LocationContext::operator=(const LocationContext &obj)
{
	if (this != &obj)
	{
		_Uri = obj._Uri;
		_Cgi = obj._Cgi;
		_AllowedMethods = obj._AllowedMethods;
		_Alias = obj._Alias;
		_LocationUri = obj._LocationUri;
		_CGI_PASS = obj._CGI_PASS;
		_Methods = obj._Methods;
		_AliasString = obj._AliasString;
		// config val data
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

LocationContext::~LocationContext() {}

LocationContext::LocationContext(const LocationContext &obj) : ConfigValues(obj)
{
	*this = obj;
}

LocationUri LocationContext::GetLocationUri() const
{
	return _LocationUri;
}

std::string LocationContext::GetAlias() const
{
	return _AliasString;
}

const ParseCGI &LocationContext::GetCGI() const
{
	return _CGI_PASS;
}

CheckMethods LocationContext::GetMethods() const
{
	return _Methods;
}

bool LocationContext::IsSet(std::string const str)
{
	int i = GetDirective(str);
	if (i < 0 || i > 8)
		throw std::invalid_argument("Error: Invalid Location Directive");
	switch (i)
	{
	case 0:
		return _AutoIndex;
	case 1:
		return _Root;
	case 2:
		return _Index;
	case 3:
		return _ClientMaxBodySize;
	case 4:
		return _ErrorPage;
	case 5:
		return _Cgi;
	case 6:
		return _AllowedMethods;
	case 7:
		return _ReturnValue;
	case 8:
		return _Alias;
	}
	throw std::invalid_argument("Error: Invalid Location Directive");
}

void LocationContext::CheckDoubleDirective(int const diractive)
{
	switch (diractive)
	{
	case 0:
	{
		if (_AutoIndex)
			throw std::invalid_argument("Error: Multiple AutoIndex expressions");
		return;
	}
	case 1:
	{
		if (_Root)
			throw std::invalid_argument("Error: Multiple Root expressions");
		if (_Alias)
			throw std::invalid_argument("Error: Multiple Alisa expressions");
		return;
	}
	case 2:
	{
		if (_Index)
			throw std::invalid_argument("Error: Multiple Index expressions");
		return;
	}
	case 3:
	{
		if (_ClientMaxBodySize)
			throw std::invalid_argument("Error: Multiple ClientMaxBodySize expressions");
		return;
	}
	case 4:
	{
		if (_ErrorPage)
			throw std::invalid_argument("Error: Multiple Error Pages");
		return;
	}
	case 5:
	{
		if (_Cgi)
			throw std::invalid_argument("Error: Multiple CGI expressions");
		return;
	}
	case 6:
	{
		if (_AllowedMethods)
			throw std::invalid_argument("Error: Multiple Server Methods expressions");
		return;
	}
	case 7:
	{
		if (_ReturnValue)
			throw std::invalid_argument("Error: Multiple Return Values");
		return;
	}
	case 8:
	{
		if (_Alias)
			throw std::invalid_argument("Error: Multiple Aliases");
		if (_Root)
			throw std::invalid_argument("Error: Multiple Root and Alias expressions");
		return;
	}
	}
}

void LocationContext::GetValueDirectivePaires(std::string data)
{
	size_t start = 0;
	size_t end = 0;
	size_t val_end = 0;
	int d = 0;
	int i = 0;

	while (data[i] != '}')
	{
		start = data.find_first_not_of(" \t\v\r\f\n", i);
		if (data[start] == '}' || start == std::string::npos)
			break;
		end = data.find_first_of(" \t\v\n\r\f", start);
		d = IsDirective(data.substr(start, end - start));
		val_end = FindVal(d, data, start, end);
		if (val_end != std::string::npos)
			i = val_end + 1;
	}
}

int LocationContext::IsDirective(std::string directive)
{
	if (_Uri == false)
		return 9;
	const int d = GetDirective(directive);
	if (d < 0 || d > 8)
		throw std::invalid_argument("Error: Invalid Directive expressions");
	else
		return d;
}

void LocationContext::SetValue(int const directive, std::string input)
{
	std::string str;
	void (LocationContext::*tab[])(std::string) = {
		&LocationContext::SetAutoIndexDirLocation,
		&LocationContext::SetRootLocation,
		&LocationContext::SetIndexLocation,
		&LocationContext::SetCMBSLocation,
		&LocationContext::SetErrorPageLocation,
		&LocationContext::SetCGI,
		&LocationContext::SetAllowedMethods,
		&LocationContext::SetReturnLocation,
		&LocationContext::SetAlias,
		&LocationContext::SetURI,
		NULL};
	str = Trim(input);
	CheckDoubleDirective(directive);
	(this->*tab[directive])(str);
}

int LocationContext::GetDirective(std::string d) const
{
	const std::string directives[] = {"autoindex", "root", "index", "client_max_body_size", "error_page", "cgi_pass", "allowed_methods", "return", "alias"};
	int dir = std::find(directives, directives + 9, d) - directives;
	return dir;
}

std::string LocationContext::Trim(std::string value)
{
	size_t start = 0;
	size_t end = 0;
	start = value.find_first_not_of(" \n\r\v\f\t");
	if (start == std::string::npos)
		return NULL;
	end = value.find_last_not_of(" \n\t\f\r\v");
	return (value.substr(start, end - start + 1));
}

size_t LocationContext::FindVal(int d, std::string str, size_t start, size_t end)
{
	std::string input;
	size_t val_end;

	if (d == 9)
	{
		input = str.substr(start, end - start);
		val_end = str.find_first_of('{', end);
	}
	else
	{
		val_end = str.find_first_of(';', end);
		if (!HasContent(';', end, val_end, str))
			throw std::invalid_argument("Error: Missing ';' charecter");
		input = str.substr(end, val_end - end);
	}
	SetValue(d, input);
	return val_end;
}

void LocationContext::SetAllowedMethods(std::string val)
{
	_AllowedMethods = true;
	CheckMethods met(val);
	_Methods = met;
}

void LocationContext::SetAlias(std::string val)
{
	_Alias = true;
	Root alias(val);
	_AliasString = val;
}

void LocationContext::SetCGI(std::string val)
{
	_Cgi = true;
	ParseCGI cgi(val);
	_CGI_PASS = val;
}

void LocationContext::SetURI(std::string val)
{
	_Uri = true;
	LocationUri uri(val);
	_LocationUri = uri;
}

void LocationContext::SetAutoIndexDirLocation(std::string val) { ConfigValues::SetAutoIndexDir(val); }
void LocationContext::SetRootLocation(std::string val) { ConfigValues::SetRoot(val); }
void LocationContext::SetIndexLocation(std::string val) { ConfigValues::SetIndex(val); }
void LocationContext::SetCMBSLocation(std::string val) { ConfigValues::SetCmbs(val); }
void LocationContext::SetErrorPageLocation(std::string val) { ConfigValues::SetErrorPage(val); }
void LocationContext::SetReturnLocation(std::string val) { ConfigValues::SetReturnDir(val); }
