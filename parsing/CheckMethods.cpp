#include "./headers/CheckMethods.hpp"

CheckMethods::CheckMethods() : _Get(false),
							   _Post(false),
							   _Delete(false) {}

CheckMethods::CheckMethods(std::string str) : _Get(false),
											  _Post(false),
											  _Delete(false)
{
	if (str.compare("") == 0)
		throw std::invalid_argument("Error: Missing Method Arguments");
	size_t args = CountArgs(str);
	if (args > 3)
		throw std::invalid_argument("Error: Too Many method Arguments");
	_Methods = ToVectorOfStrings(str);
	ValidateMethods();
}

CheckMethods::~CheckMethods() {}

CheckMethods &CheckMethods::operator=(const CheckMethods &obj)
{
	if (this != &obj)
	{
		_Get = obj._Get;
		_Post = obj._Post;
		_Delete = obj._Delete;
		_Methods = obj._Methods;
	}
	return *this;
}

CheckMethods::CheckMethods(const CheckMethods &obj) : _Get(obj._Get),
													  _Post(obj._Post),
													  _Delete(obj._Delete),
													  _Methods(obj._Methods) {}

bool CheckMethods::GetGET() const
{
	return _Get;
}
bool CheckMethods::GetPost() const
{
	return _Post;
}
bool CheckMethods::GetDelete() const
{
	return _Delete;
}
std::vector<std::string> CheckMethods::GetMethods() const
{
	return _Methods;
}

void CheckMethods::ValidateMethods()
{
	for (size_t i = 0; i < _Methods.size(); i++)
	{
		if (_Methods.at(i).empty())
			break;
		switch (IsValidHttpMethod(_Methods.at(i)))
		{
		case 0:
			_Get = true;
			break;
		case 1:
			_Post = true;
			break;
		case 2:
			_Delete = true;
			break;
		}
	}
}
