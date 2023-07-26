/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 18:33:36 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/20 22:05:35 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "./../http/Request.hpp"

class CGI {

	public:
		CGI(Request const &, short const &, std::string const &);
		~CGI();
		CGI(const CGI &obj);

		void setEnv();
		void Exec();
		std::string &GetOutput();
		std::string const &GetExtention() const{
			return _ScriptExtension;
		}

	private:
		std::string ParsePath(std::string input);
		std::string ParseScriptName(std::string input);
		std::string GetScriptExtention(std::string input);
		CGI &operator=(const CGI &obj);
		Request const & _Request;
		short _Port;
		char *env[14];
		std::string _ScriptName;
		std::string _ScriptExtension;
		std::string _Root;
		std::string _CgiPath;
		std::string _CgiOutput;
		std::string	_Method;
};

#endif

