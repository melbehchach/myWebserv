#include "./headers/CGI.hpp"

std::ostream & operator<< (std::ostream & o, std::vector<char *> const & v) {
	for(size_t i =0; i< v.size(); i++) {
		o << v[i] << "\n";
	}
	return o;
}

const std::string names[] = {
	"REDIRECT_STATUS", "DOCUMENT_ROOT", "SERVER_SOFTWARE", "SERVER_PORT", "GATEWAY_INTERFACE", "SERVER_NAME", "SCRIPT_FILENAME", "REQUEST_METHOD", "SERVER_PROTOCOL", "CONTENT_TYPE", "CONTENT_LENGTH", "PATH_INFO", "QUERY_STRING", "HTTP_COOKIE"};

CGI::CGI(cgiData const &data, std::string const &port, std::string const & q) : _Data(data), _Request(data.req),  _Port(port), _Query(q), _Method(data.req._method)
{
	this->_ScriptName = this->ParseScriptName(_Query);
	this->_CgiPath = data.location.GetCGI().GetFilePath();
	this->_ScriptExtension = data.location.GetCGI().GetFileExtention();
	if (_ScriptExtension.compare("") == 0)
		_ScriptExtension = GetScriptExtention(this->_CgiPath);
	_Root = data.location.GetRoot();
	_Root.append(data.location.GetLocationUri().GetUri());

	if (access(this->_CgiPath.c_str(), F_OK) == -1)
		throw std::runtime_error("Error: Missing File Or Directory \"" + _CgiPath + "\"");
	if (access(this->_CgiPath.c_str(), X_OK) == -1)
		throw std::runtime_error("Error: Invalid Permissions \"" + _CgiPath + "\"");

	if (access((_Root + "/scripts/" + _ScriptName).c_str(), F_OK) == -1)
		throw std::runtime_error("Error: Missing File Or Directory \"" + (_Root + "/scripts/" + _ScriptName) + "\"");
	if (access((_Root + "/scripts/" + _ScriptName).c_str(), X_OK) == -1)
		throw std::runtime_error("Error: Invalid Permissions \"" + (_Root + "/scripts/" + _ScriptName) + "\"");
	this->setEnv();
	this->Exec();
}

CGI::CGI(const CGI &obj) : _Data(obj._Data),
							_Request(obj._Request),
						   _Root(obj._Root),
						   _CgiPath(obj._CgiPath),
						   _CgiOutput(obj._CgiOutput) {}

CGI::~CGI() {}

std::string &CGI::GetOutput()
{
	return this->_CgiOutput;
}

void CGI::setEnv()
{
	std::string str = _Root;
	str.append("/scripts/" + _ScriptName);

	if (0 > setenv("DOCUMENT_ROOT", _Root.c_str(), 1) || 0 > setenv("SERVER_PORT", _Port.c_str(), 1) || 0 > setenv("GATEWAY_INTERFACE", "CGI/1.1", 1) || 0 > setenv("SERVER_NAME", _Data.server.GetServerNames()[0].c_str(), 1) || 0 > setenv("SCRIPT_FILENAME", str.c_str(), 1) || 0 > setenv("REQUEST_METHOD", _Method.c_str(), 1) || 0 > setenv("QUERY_STRING", _Query.c_str(), 1) || 0 > setenv("SERVER_PROTOCOL", "HTTP/1.1", 1) || 0 > setenv("SERVER_SOFTWARE", "prj dial 13", 1) || 0 > setenv("PATH_INFO", _CgiPath.c_str(), 1) || 0 > setenv("REDIRECT_STATUS", "200", 1) || 0 > setenv("REQUEST_METHOD", _Method.c_str(), 1))
	{
		throw std::runtime_error("Error: faild to set enviroment varialble");
	}
	std::map<std::string, std::string>::const_iterator tmp = _Data.req._msgrequest.find("Cookie");
	if (tmp != _Data.req._msgrequest.end())
	{
		if (0 > setenv("HTTP_COOKIE", tmp->second.c_str(), 1))
			throw std::runtime_error("Error: faild to set enviroment varialble");
	}
	if (_Data.req._msgrequest.find("Content_Type") != _Data.req._msgrequest.end())
	{
		if (0 > setenv("CONTENT_TYPE", _Data.req._msgrequest.find("Content_Type")->second.c_str(), 1))
			throw std::runtime_error("Error: faild to set enviroment varialble");
	}
	if (_Data.req._msgrequest.find("Content_Length") != _Data.req._msgrequest.end())
	{
		if (0 > setenv("CONTENT_LENGTH", _Data.req._msgrequest.find("Content_Length")->second.c_str(), 1))
			throw std::runtime_error("Error: faild to set enviroment varialble");
	}
}

void CGI::Exec()
{
	// std::vector<char *>env;
	char  **env = new char*[14];
	char const *args[3];
	int write_fd[2];
	int read_fd[2];
	pid_t ChildId;
	char buff[1025];
	int ReadCount = 0;
	_Root.append("/scripts");
	_CgiPath = ParsePath(_CgiPath);
	args[0] = _CgiPath.c_str();
	args[1] = getenv("SCRIPT_FILENAME");
	if (args[1] == NULL)
		throw std::invalid_argument("Error: Can't find \"Script Name\" enviroment varialble.");
	args[2] = NULL;
	if (0 > pipe(write_fd) || 0 > pipe(read_fd))
		throw std::runtime_error("Error: Filed To Creat Discriptor Pair.");
	int k = 0;
	for (size_t i = 0; i < 14; i++)
	{
		std::string full = names[i];
		char *str = getenv(names[i].c_str());
		if (str == NULL)
		{
			continue;
		}
		full += "=";
		full += str;
		// env.push_back(strdup(full.c_str()));
		// char tmp[10000];
		// size_t j;
		// for (j = 0; j < full.length() + 1; j++)
		// 	tmp[j] = full[j];
		// tmp[j] = '\0';
		env[k] = strdup(full.c_str());
		k++;
	}
	env[k] = NULL;
	ChildId = fork();
	if (ChildId < 0)
		throw std::runtime_error("Error: Can't Creat Child Prosses");
	if (ChildId == 0)
	{
		dup2(write_fd[0], 0); // stdin
		close(write_fd[1]);
		dup2(read_fd[1], 1); // stdout
		close(read_fd[0]);
		chdir(_Root.c_str());
		if (execve(_CgiPath.c_str(), (char *const *)args, env) == -1)
		{
			std::cout << strerror(errno);
			exit(1);
		}
	}
	else
	{
		int status = 0;
		int timeout = 3;
		int result = 0;
		while (result == 0 && timeout > 0)
		{
			result = waitpid(ChildId, &status, WNOHANG);
			if (result > 0)
			{
				break;
			}
			if (result == 0)
			{
				sleep(1);
				timeout--;
			}
		}

		close(read_fd[1]);
		close(write_fd[0]);
		for (size_t i = 0 ; env[i] != NULL; i++) {
			delete env[i];
		}
		delete [] env;

		if (result == 0)
		{
			printf("Child process took too long, killing it\n");
			kill(ChildId, SIGKILL);
			throw std::runtime_error("Child process took too long");
		}
		std::string body;
		for (size_t i = 0; i < this->_Request._body.size(); i++)
		{
			body += this->_Request._body[i];
		}
		if (body.size() != 0)
			write(write_fd[1], body.c_str(), body.size());
		close(write_fd[1]);
		bzero(buff, 1024);
		while ((ReadCount = read(read_fd[0], buff, 1024)) > 0)
		{
			buff[ReadCount] = '\0';
			_CgiOutput.append(buff, ReadCount);
		}
		close(read_fd[0]);
	}
	std::cout << "done executing cgi\n";
	std::cout << "out == [" << this->_CgiOutput << "]\n";
	// std::cout << env << "\n";
}

std::string CGI::ParsePath(std::string input)
{
	size_t pos = input.find_last_of("/");
	pos++;
	std::string tmp = input.substr(pos);
	if (tmp.compare("php-cgi") == 0)
		return "../" + tmp;
	else if (tmp.compare("python") == 0)
		return "/usr/bin/python";
	throw std::invalid_argument("Error: Bad CGI");
}
std::string CGI::ParseScriptName(std::string input)
{
	if (input.compare("") == 0)
		throw std::invalid_argument("Error: Bad Querry");
	// size_t pos = input.find_last_of("/");
	// pos++;
	return input;
}
std::string CGI::GetScriptExtention(std::string input)
{
	size_t pos = input.find_last_of("/");
	pos++;
	std::string tmp = input.substr(pos);
	if (tmp.compare("python") != 0 && tmp.compare("php-cgi") != 0)
		throw std::invalid_argument("Error: CGI Extention Not Supported");
	return tmp;
}
