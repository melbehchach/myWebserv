/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 16:49:22 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/19 22:03:10 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTEXT_HPP
#define SERVERCONTEXT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstddef> // size_t
#include <stdlib.h> // exit()
#include <utility> // std::pair
#include <vector>
#include <map>
#include "./LocationContext.hpp"
#include "./ConfigValues.hpp"
#include "./ServerName.hpp"
#include "./Listen.hpp"

class ServerContext : public ConfigValues {
	private:
		size_t _LocationPos;
		bool _IsListening;
		bool _CustomServerName;
		size_t _ServerID;
		std::multimap<std::string, std::string> _Listen;
		std::vector<std::string> _ServerNames;
		std::vector<LocationContext> _LocationContext;

		size_t	FindLocationContextEnd(std::string ConfigFile, size_t Pos);
		void GetDirectiveValuePair(size_t *pos, std::string file);

		void SetServerRoot(std::string val);
		void SetServerIndex(std::string val);
		void SetServerCMBS(std::string val);
		void SetServerErrorPage(std::string val);
		void SetServerAutoIndexDir(std::string val);
		void SetServerReturn(std::string val);

		int IsDirective(std::string const directive);
		int GetDirective(std::string const directive);
		void DoubleDirectiveCheck(int directive);

		size_t FindVal(int Directive, std::string ConfigFile, size_t End);
		void SetValue(int directive, std::string value); // override func


		void SetServerLocation(std::string val);
		void SetServerListen(std::string val);
		void SetServerName(std::string val);

		bool HasContent(char EndChar, size_t EndPos, size_t EndValue, std::string ConfigFile);
		std::string Trim(std::string value);

	public:
		ServerContext();
		~ServerContext();
		ServerContext(size_t *start, std::string file, size_t ServerId);
		ServerContext(const ServerContext &obj);
		ServerContext &operator=(const ServerContext& obj);


		bool HasLocation(std::string target);
		virtual bool IsSet(std::string directive);

		const std::vector<LocationContext>& GetLocationContexts() const;
		const std::multimap<std::string, std::string> &GetListen()const;
		std::string GetIpAddress() const;
		std::vector<std::string> GetPortNumbers() const;
		std::vector<std::string> GetServerNames() const;

		void DeletePort(std::multimap<std::string,std::string>::iterator it);

};


#endif
