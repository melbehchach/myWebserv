/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/01 11:54:56 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/12 01:03:04 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

class ConfigFileParser;

size_t StringToSize_T (std::string str);
size_t CountArgs(std::string val);
bool IsNumber(std::string input);
std::vector<std::string> ToVectorOfStrings(std::string const &str) ;
bool SetAutoIndex(std::string val) ;
bool IsUri(std::string str) ;
bool IsDirectory(std::string str);
bool IsHexDig(int i);
bool IsValidString(int (*func) (int),std::string const &str, std::string const &allow) ;
bool IsIpv4Format(std::string const &s) ;
bool IsUnreserved(int c);
bool IsSubDelim(int c);
bool IsUnreservedSubDelim(char c) ;
std::string DecodePercent(std::string const &str, size_t pos) ;
bool IsPChar(char c);
int IsValidHttpMethod(std::string str) ;
bool IsPath(std::string path) ;
bool HasContent(char end_char, size_t key_end, size_t value_end, std::string config);
bool ValidDecOctetGroop(std::string const &str) ;
void CheckString(int (*func)(int), std::string &str, size_t pos);
void CheckDirectoryValidity(const ConfigFileParser &obj);

#endif
