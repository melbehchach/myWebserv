/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValues.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/30 12:02:07 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/08 17:18:18 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// this class stores all the vals in server block

#ifndef CONFIGVALUES
#define CONFIGVALUES

#include "./Index.hpp"
#include "./ReturnDir.hpp"
#include "./Root.hpp"
#include "./ClientMaxBodySize.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>

// cant creat this obj , can only inharit its atributes

class ConfigValues {
	protected:
		bool _Root;
		bool _Index;
		bool _ClientMaxBodySize;
		bool _ErrorPage;
		bool _AutoIndex;
		bool _ReturnValue;

		std::string _RootPath;
		std::vector<std::string> _IndexList;
		size_t _ClientMaxSize;
		std::map<int, std::string> _ErrorPages;
		bool _AutoIndexStatus;
		ReturnDir _ReturnDir;



		//setters
		void SetRoot(std::string val);
		void SetIndex(std::string val);
		void SetCmbs(std::string val);
		void SetErrorPage(std::string val);
		void SetAutoIndexDir(std::string val);
		void SetReturnDir(std::string val);

		virtual void SetValue(int const directive, std::string val) = 0;
		virtual int IsDirective(std::string const directive) = 0;

		ConfigValues(std::string str);
		virtual ~ConfigValues();
		ConfigValues(const ConfigValues &obj);
		ConfigValues &operator=(const ConfigValues& obj);

		// void updateRoot(std::string str) { // fix function (dont call)
		// 	this->_RootPath = str;
		// }

	public:

		virtual std::string GetRoot() const;
		virtual std::vector<std::string> GetIndex() const;
		virtual size_t GetCmbs() const;
		virtual std::map<int, std::string> GetErrorPage() const;
		virtual bool GetAutoIndexDir() const;
		virtual ReturnDir GetReturn() const;
		virtual bool HasErrorPage() const ;
		void	AddToErrorPageMap(std::map<int, std::string> *map, std::string input);
		virtual bool IsSet(std::string directive) = 0;

};




#endif
