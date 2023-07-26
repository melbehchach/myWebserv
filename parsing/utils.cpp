/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 18:21:47 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/12 01:16:55 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/utils.hpp"
#include "./headers/ConfigFileParser.hpp"

size_t StringToSize_T(std::string str)
{
	size_t number = 0;
	size_t i = str.find_first_not_of(" \n\t\r\v\f");
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			throw std::invalid_argument("Error: Negative Number");
		i++;
	}
	while (std::isdigit(str[i]) && str[i])
	{
		number = (number * 10) + (str[i] - '0');
		i++;
	}
	return number;
}

size_t CountArgs(std::string val)
{
	size_t i = 0;
	size_t start = 0;
	size_t end = 0;
	size_t args = 0;

	while (val[i])
	{
		start = val.find_first_not_of(" \n\t\r\v\f", i);
		if (start == std::string::npos)
			return args;
		end = val.find_first_of(" \n\t\v\f\r", start);
		args++;
		if (end == std::string::npos)
			break;
		i = end;
	}
	return args;
}

bool IsNumber(std::string input)
{
	size_t i = 0;
	while (input[i])
	{
		if (std::isdigit(input[i]) == 0)
			return false;
		i++;
	}
	return true;
}

std::vector<std::string> ToVectorOfStrings(std::string const &str)
{
	size_t start = 0;
	size_t end = 0;
	std::string key;
	std::vector<std::string> vec;
	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		start = str.find_first_not_of(" \n\r\t\f\v", i);
		if (start == std::string::npos)
			break;
		end = str.find_first_of(" \n\t\v\f\r", start);
		key = str.substr(start, end - start);
		vec.push_back(key);
		if (end == std::string::npos)
			break;
		i = end;
	}
	return vec;
}

bool SetAutoIndex(std::string val)
{
	if (val.compare("") == 0)
		throw std::invalid_argument("Error: Missing AutoIndex Argument");
	if (CountArgs(val) > 1)
		throw std::invalid_argument("Error: (AutoIndex) Invalid Number of Argument");
	if (val.compare("on") != 0 && val.compare("off") != 0)
		throw std::invalid_argument("Error: Invalid AutoIndex Argument");
	if (val.compare("on") == 0)
		return true;
	else if (val.compare("off") == 0)
		return false;
	throw std::invalid_argument("Error: Bad AutoIndex");
}

bool IsUri(std::string str)
{
	if (str[0] == '/')
		return true;
	return false;
}

bool IsDirectory(std::string str)
{
	if (str[str.size() - 1] == '/')
		return true;
	return false;
}

bool IsHexDig(int i)
{
	if (std::isdigit(i) || (i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z'))
		return true;
	return false;
}

// check if all elems of str satisfy execpt for the elems in allow
bool IsValidString(int (*func)(int), std::string const &str, std::string const &allow)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (func(str[i]) == false && allow.find(str[i]) == std::string::npos)
			return false;
	}
	return true;
}

bool IsIpv4Format(std::string const &s)
{
	if (IsValidString(std::isdigit, s, ".:-") && std::count(s.begin(), s.end(), '.') > 1)
		return true;
	return false;
}

bool IsUnreserved(int c)
{
	return (c == '-' || c == '.' || c == '_' || c == '~' || std::isalpha(c) || std::isdigit(c));
}

bool IsSubDelim(int c)
{
	return (c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=');
}

bool IsUnreservedSubDelim(char c)
{
	return (IsUnreserved(c) || IsSubDelim(c));
}

// loop through str and convert any percent encoded val (starting with %)
std::string DecodePercent(std::string const &str, size_t pos)
{
	std::string s;
	// if pos if given copy copy all preceding charecters
	if (pos > 0)
		s = str.substr(0, pos);

	for (std::string::const_iterator it = str.begin() + pos; it != str.end(); ++it)
	{
		if (*it == '%')
		{ // percent encoded vals are preceded by %
			std::string hex = std::string(it + 1, it + 3);
			if (hex == "0D" || hex == "0A")
				throw std::invalid_argument("Error: Bad Uri Request");
			char c = std::stoi(hex, NULL, 16);
			s += c;
			it += 2; // skip past percent incoded value
		}
		else
			s += *it;
	}
	return s;
}

// absolute path are at least one '/' followed bt pchars
bool IsPChar(char c)
{
	if (c == ':' || c == '@' || IsUnreserved(c) || IsSubDelim(c))
		return true;
	return false;
}

int IsValidHttpMethod(std::string str)
{
	std::string methods[] = {"GET", "POST", "DELETE"};
	int is_method = std::find(methods, methods + 3, str) - methods;
	if (is_method < 0 || is_method > 2)
		throw std::invalid_argument("Error: Wrong Method Found");
	return is_method;
}

bool IsPath(std::string path)
{
	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i] == '/')
			return true;
	}
	return false;
}

bool HasContent(char end_char, size_t key_end, size_t value_end, std::string config)
{
	size_t i = 0;
	while (std::isspace(config[key_end + i]) && key_end + i < value_end)
	{
		i++;
	}
	if (config[key_end + i] == end_char)
		return false;
	return true;
}

// check if dec-octet group range
bool ValidDecOctetGroop(std::string const &str)
{
	size_t begin = str.find_last_of('.');
	int octet;

	if (begin == std::string::npos)
	{ // if in first groop
		if (str.find(":") != std::string::npos)
		{ // if part of ipv6 address
			size_t start = str.find_last_of(":") + 1;
			octet = std::stoi(str.substr(start));
		}
		else
			octet = std::stoi(str);
	}
	else // 2nd groop
		octet = std::stoi(str.substr(begin + 1));
	if (octet < 0 || octet > 255)
		return false;
	return true;
}

void CheckString(int (*func)(int), std::string &str, size_t pos)
{
	std::transform(str.begin() + pos, str.end(), str.begin() + pos, func);
}

void CheckDirectoryValidity(const ConfigFileParser &obj)
{
	std::string str;
	for (size_t i = 0; i < obj.GetNumberOfServers(); i++)
	{
		for (size_t j = 0; j < obj.GetServers()[i].GetLocationContexts().size(); j++)
		{
			str = obj.GetServers()[i].GetLocationContexts()[j].GetRoot() + obj.GetServers()[i].GetLocationContexts()[j].GetLocationUri().GetInputURI();
			if (str.back() == '/')
				str.pop_back();
			int fd = open(str.c_str(), O_RDONLY);
			int fd2 = open(str.c_str(), O_WRONLY);
			if (fd == -1 && fd2 == -1)
			{
				close(fd);
				close(fd2);
				throw std::invalid_argument("Error: Missing Directory [" + str + "]");
			}
			else if (fd != -1 && fd2 != -1)
			{
				close(fd);
				close(fd2);
				throw std::invalid_argument("Error: Location Uri Has To Be A Directory [" + str + "]");
			}
			close(fd);
			close(fd2);
		}
	}
}
