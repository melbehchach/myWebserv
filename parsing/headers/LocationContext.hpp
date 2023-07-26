/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 09:46:40 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/19 22:02:37 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONTEXT_HPP
#define LOCATIONCONTEXT_HPP

#include "./CheckMethods.hpp"
#include "./ConfigValues.hpp"
#include "./LocationUri.hpp"
#include "./ParseCGI.hpp"
#include "./utils.hpp"
#include <iostream>
#include <exception>
#include <fcntl.h>
#include <unistd.h>

class LocationUri;

class LocationContext : virtual public ConfigValues {

	public:
		LocationContext(std::string root);

		LocationContext(std::string str, std::string root);

		LocationContext &operator=(const LocationContext &obj) ;
		virtual ~LocationContext();
		LocationContext(const LocationContext &obj) ;

		LocationUri GetLocationUri() const;
		std::string GetAlias() const ;
		const ParseCGI &GetCGI() const ;
		CheckMethods GetMethods() const;

		virtual bool IsSet(std::string const str) ;
		// void passRoot(std::string str) { // fix function (dont call
		// 	updateRoot(str);
		// }

	protected:
		bool _Uri;
		bool _Cgi;
		bool _AllowedMethods;
		bool _Alias;
		LocationUri _LocationUri;
		ParseCGI _CGI_PASS;
		CheckMethods _Methods;
		std::string _AliasString;

		void CheckDoubleDirective(int const diractive) ;
		void GetValueDirectivePaires(std::string data);

		// overlodes
		virtual int IsDirective(std::string directive) ;
		virtual void SetValue(int const directive, std::string input) ;

		int GetDirective(std::string d) const ;

		std::string Trim(std::string value) ;

		// setters
		size_t FindVal(int d, std::string str, size_t start, size_t end) ;

		void SetAllowedMethods(std::string val) ;

		void SetAlias(std::string val) ;

		void SetCGI(std::string val);

		void SetURI(std::string val) ;

		void SetAutoIndexDirLocation(std::string val);
		void SetRootLocation(std::string val) ;
		void SetIndexLocation(std::string val) ;
		void SetCMBSLocation(std::string val);
		void SetErrorPageLocation(std::string val);
		void SetReturnLocation(std::string val);

};

#endif
