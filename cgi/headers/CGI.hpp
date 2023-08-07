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
#include <signal.h>
#include "./../../request/request.hpp"
#include "./../../parsing/headers/LocationContext.hpp"
#include "./../../parsing/headers/ServerContext.hpp"

typedef struct configData {
    LocationContext const & location;
    ServerContext const & server;
    request const & req;
	configData(LocationContext const & l, ServerContext const & s, request const&  r) : location(l), server(s), req(r) {}
} cgiData;

class CGI {

	public:
		CGI(cgiData const &, std::string const &, std::string const &);
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
		cgiData const & _Data;
		request const & _Request;
		const std::string _Port;
		const std::string _Query;
		char *env[14];
		std::string _ScriptName;
		std::string _ScriptExtension;
		std::string _Root;
		std::string _CgiPath;
		std::string _CgiOutput;
		const std::string	_Method;
};

#endif

