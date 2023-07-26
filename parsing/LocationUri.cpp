/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationUri.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:55:06 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/11 20:16:29 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/LocationUri.hpp"

LocationUri::LocationUri() : _Uri("/"), _IsDir(true) {}

LocationUri::LocationUri(std::string str) : _Uri(str)
{
	if (str.compare("") == 0)
		throw std::invalid_argument("Error: Empty Uri");
	if (IsUri(str) == false)
		throw std::invalid_argument("Error: invalid Uri");
	_IsDir = IsDirectory(str);
}

LocationUri::~LocationUri() {}

LocationUri::LocationUri(const LocationUri &obj) : _Uri(obj._Uri), _IsDir(obj._IsDir) {}

LocationUri &LocationUri::operator=(LocationUri const &obj)
{
	if (this != &obj)
	{
		_Uri = obj._Uri;
		_IsDir = obj._IsDir;
	}
	return *this;
}

std::string LocationUri::GetUri() const
{
	return _Uri.GetParsedURI();
}

Uri LocationUri::GetUriOBJ() const
{
	return _Uri;
}

std::string LocationUri::GetInputURI() const
{
	return _Uri.GetInputURI();
}

bool LocationUri::IsDir() const
{
	return _IsDir;
}
