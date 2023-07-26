/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UriParser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 13:33:20 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/11 20:22:21 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URIPARSER
#define URIPARSER

#include "./HostStateParser.hpp"
#include "./Uri.hpp"
#include "./utils.hpp"

enum HostState {
	h_start,
	h_literal,
	h_Ipv6,
	h_IpvF,
	h_Ipv4,
	h_literalend,
	h_regname,
	h_regnamepct,
	h_regnamepctdone,
	h_port,
	h_done,
	h_invalid
};


class Uri;

/*
	parsing the host uri string into uri host string
	dose not eccept comma delimited list of multiple hosts "split the str first"
	b4 passing to parser
*/


class UriParser : public HostStateParser<HostState> {
	public:

		UriParser();
		~UriParser();
		size_t Parse(Uri &uri, std::string const &inpt);

	private:

		HostState PushBuffer(std::string &field , HostState next);
		HostState LiteralHandler(size_t index);
		HostState RegNameHndler(size_t index);
		HostState IpvFHndler(size_t index);
		HostState LiteralEndHndler(size_t index);
		HostState Ipv6Hndler(size_t index);
		HostState PortHndler(size_t index);
		HostState RegNamePctDoneHndler(size_t index);
		HostState RegNamePctHndler(size_t index);
		HostState Ipv4Hndler(size_t index);

		HostState StartHandler(size_t index);

		Uri *_Uri;
		size_t _Groops; // counting groups of IPV4 sequences
		size_t _Colons; // keep track of colons in ipv6
		size_t _Numbers; // counting nb id digits in a groop
		bool _Lit;

		protected:
			HostState  GetNextState(size_t index);
			void CheckInvalidState() const;
			void AfterParserCheck();

};

HostState Handelipv4Digits(size_t &nums, size_t &groops, std::string const & buf);
bool ValidLastBitsIpv4(std::string const &str, size_t pos);
HostState HandelIpv6Digit(size_t &colons, size_t &digit);
HostState HandelIpv6Colone(size_t & colons , size_t &numbers, size_t &groops);
void NormalizeIPv6HexDig(std::string &buff, char c, bool &skip_c);

#endif
