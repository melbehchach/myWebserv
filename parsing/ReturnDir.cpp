/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReturnDir.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 17:57:00 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/11 16:06:23 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/ReturnDir.hpp"


bool ReturnDir::ValidReturnCode(size_t code){
		if (code < 300 || code > 399)
			return false;
		return true;
	}

void ReturnDir::FillReturn(std::string input) {
	size_t i = 0;
	size_t start = 0;
	size_t end = 0;
	size_t set = 0;
	std::string key;
	while (input[i]) {
		start = input.find_first_not_of(" \n\r\t\v\f", i);
		if (start == std::string::npos)
			break;
		end = input.find_first_of(" \t\v\n\r\f", start);
		key = input.substr(start, end - start);
		SetVariables(key, &set);
		if (end == std::string::npos)
			break;
		i = end;
	}
}

void ReturnDir::SetReturnCode(std::string key) {
	_Code = StringToSize_T(key);
	if (!ValidReturnCode(_Code))
		throw std::invalid_argument("Error: Invalid Return Code");
}

void ReturnDir::SetVariables(std::string key, size_t *set) {
	if (IsNumber(key) == false && *set == 0)
		throw std::invalid_argument("Error: Invalid Return Code");
	if (*set == 0) {
		SetReturnCode(key);
		*set += 1;
	}
	else if (*set > 1)
		throw std::invalid_argument("Error: Too Many Return Values");
	else {
		_Url = key;
		*set += 1;
	}
}

ReturnDir::ReturnDir() : _Is_Set(false) {}

ReturnDir::ReturnDir(std::string input) : _Is_Set(false) {
	if (input.compare("") == 0)
		throw std::invalid_argument("Error: Missing return argument");
	FillReturn(input);
}

ReturnDir::ReturnDir(ReturnDir const &obj): _Is_Set(obj._Is_Set), _Code(obj._Code), _Url(obj._Url) {}

ReturnDir &ReturnDir::operator=(const ReturnDir &obj) {
	if (this != &obj) {
		_Is_Set = obj._Is_Set;
		_Code = obj._Code;
		_Url = obj._Url;
	}
	return *this;
}

ReturnDir::~ReturnDir() {}

int ReturnDir::GetCode() const {
	return _Code;
}

std::string ReturnDir::GetUrl()const {
	return _Url;
}

bool ReturnDir::IsSet() const {
	return _Is_Set;
}
