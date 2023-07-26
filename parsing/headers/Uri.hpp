/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 17:58:36 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/16 17:07:56 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URI_HPP
#define URI_HPP

#include "./UriParser.hpp"
#include "./TargetRequestParser.hpp"
#include <string>

class Uri {
	public:

		friend class UriParser;
		friend class TargetRequestParser;

		enum Part {
			Error = -1,
			Host = 0,
			Path = 1,
			Query = 2
		};
		Uri();
		explicit Uri(std::string const &str);
		~Uri() ;
		Uri(const Uri &obj) ;
		Uri &operator=(const Uri &obj) ;
		Uri &operator=(std::string const & str);

		std::string const&	GetHost() const ;
		std::string const &GetPath() const ;
		std::string const &GetQuery() const ;
		std::string const & Get()const ;
		std::string const & GetPort()const ;
		std::string const & GetParsedURI()const ;
		std::string const & GetInputURI()const ;
		std::string GetHostWithPort()const ;

		void SetHost(std::string const &str);
		void SetPath(std::string const &str);
		void SetQuery(std::string const &str);

	private:
		// parses URI depending on staring charecter of input str
		// part uesd to specify which parser to use
		void ParseInput(Part val = static_cast<Part>(-1));

		std::string ConstructParsedUri();

		std::string _UriInput;
		std::string _UriParsed;
		std::string _Host;
		std::string _Port;
		std::string _Path;
		std::string _Query;
};

#endif
