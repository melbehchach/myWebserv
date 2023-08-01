#include "./headers/Uri.hpp"


Uri::Uri() {}

Uri::Uri(std::string const &str) :
_UriInput(str) {
	ParseInput();
}

Uri::~Uri() {}

Uri::Uri(const Uri &obj) {
	*this = obj;
}

Uri &Uri::operator=(const Uri &obj) {
	if (this != &obj) {
		this->_UriInput = obj._UriInput;
		this->_UriParsed = obj._UriParsed;
		this->_Host = obj._Host;
		this->_Port = obj._Port;
		this->_Path = obj._Path;
		this->_Query = obj._Query;
	}
	return *this;
}

Uri &Uri::operator=(std::string const & str){
	_Host.clear();
	_Port.clear();
	_Path.clear();
	_Query.clear();
	_UriInput = str;
	ParseInput();
	return *this;
}

std::string const&	Uri::GetHost() const {
	return _Host;
}

std::string const &Uri::GetPath() const {
	return _Path;
}

std::string const &Uri::GetQuery() const {
	return _Query;
}

std::string const & Uri::Get()const {
	return _UriParsed;
}

std::string const & Uri::GetPort()const {
	return _Port;
}

std::string const & Uri::GetParsedURI()const {
	return _UriParsed;
}

std::string const & Uri::GetInputURI()const {
	return _UriInput;
}

std::string Uri::GetHostWithPort()const {
	if (_Port.empty())
		return _Host;
	else
		return _Host + ":" + _Port;
}

void Uri::SetHost(std::string const &str) {
	_UriInput = str;
	ParseInput(static_cast<Part>(0));
}

void Uri::SetPath(std::string const &str) {
	_UriInput = str;
	ParseInput(static_cast<Part>(1));
}

void Uri::SetQuery(std::string const &str) {
	_Query = str;
	_UriParsed = ConstructParsedUri();
}

// parses URI depending on staring charecter of input str
// part uesd to specify which parser to use
void Uri::ParseInput(Part val) {
	size_t pos = _UriInput.find('/');
	// if no '/' found assume its a host string
	if (val == static_cast<Part>(0) || pos == std::string::npos) {
		UriParser parser;
		parser.Parse(*this, _UriInput);
	}
	// if starts with '/' assumes its origin from uri
	else if (val == static_cast<Part>(1) || pos == 0) {
		TargetRequestParser parser;
		parser.Parse(*this, _UriInput);
	}
	else
		throw std::invalid_argument("Error: Bad Uri Format");
	_UriParsed = ConstructParsedUri();
}

std::string Uri::ConstructParsedUri() {
	std::string uri;
	if (_Host.empty() == false)
		uri += GetHostWithPort();
	if (_Path.empty() == false)
		uri += GetPath();
	if (_Query.empty() == false)
		uri += "?" + GetQuery();
	return uri;
}
