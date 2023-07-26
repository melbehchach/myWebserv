/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TargetRequestParser.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 14:21:51 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/14 18:41:19 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TARGETREQUESTPARSER_HPP
#define TARGETREQUESTPARSER_HPP

#include "./HostStateParser.hpp"
#include "./Uri.hpp"

enum State {
	u_start,
	u_path,
	u_query,
	u_percent,
	u_percent_done,
	u_done,
	u_invalid
};


enum Part {
	Error = -1,
	Host = 0,
	Path = 1,
	Query = 2
};

class Uri;

class TargetRequestParser : public HostStateParser<State> {

	private:

		State StartHandler(char c);
		State PathHandler(char c) ;
		State QueryHandler(char c) ;
		State PercentHandler(char c);
		State PercentDoneHandler(char c) ;
		State PushBufferToField(State next, bool skip = true) ;

	public:

		TargetRequestParser() ;
		~TargetRequestParser();
		size_t Parse(Uri &uri, std::string const &input) ;

	protected:
		State GetNextState(size_t index);
		void CheckInvalidState() const ;
		void PreParseCheck();
		void AfterParserCheck();

		private:
			Uri *_Uri;
			Part _Part;


};






#endif
