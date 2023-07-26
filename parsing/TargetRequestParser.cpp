/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TargetRequestParser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 18:05:52 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/08 18:31:32 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/TargetRequestParser.hpp"

// starting state transition handler, only accepts '/'
State TargetRequestParser::StartHandler(char c)
{
	if (c == '/')
		return u_path;
	return u_invalid;
}

// handels transition after '/' input indicating path section in uri
// always checks that no 2 consecutive '/' are given
// only used by autority uri conponents
State TargetRequestParser::PathHandler(char c)
{
	_Part = static_cast<Part>(1);
	switch (c)
	{
	case '\0':
		return PushBufferToField(u_done);
	case '%':
		return u_percent;
	case '?':
		return PushBufferToField(u_query);
	case '/':
		if (_Buf.back() != '/')
			return u_path;
		break;
	default:
		if (IsPChar(c))
			return u_path;
		else
			return u_invalid;
	}
	return u_invalid;
}

// handels transition after '?' input indicating queries is found
// '#' is accepted alternative to EOL signaling end of query string
State TargetRequestParser::QueryHandler(char c)
{
	_Part = static_cast<Part>(2);
	switch (c)
	{
	case '\0':
	case '#':
		return PushBufferToField(u_done);
	case '%':
		return u_percent;
	case '/':
	case '?':
		return u_query;
	default:
		if (IsPChar(c))
			return u_query;
		else
			return u_invalid;
	}
}
// handles transition after '%' has been found
// checkes if sebsequent 2 chars are valid hexa digits
State TargetRequestParser::PercentHandler(char c)
{
	if (_Buf.back() == '%' && IsHexDig(c))
		return u_percent;
	else if (IsHexDig(_Buf.back()) && IsHexDig(c))
		return u_percent_done;
	return u_invalid;
}

// handels transition after %HH
// pchar & '/' input trigger transition to either query or path state
// depending on if were on the path or query part of uri
// "return State(_Path)" uses equivalency between Path & Query values
// in Uri::Path enum and u_path and query state values in uristate enum
// if at path part we return path state same for query
State TargetRequestParser::PercentDoneHandler(char c)
{
	CheckString(std::toupper, _Buf, _Buf.size() - 2);
	_Buf = DecodePercent(_Buf, _Buf.size() - 3);
	switch (c)
	{
	case '\0':
		return PushBufferToField(u_done);
	case '#':
		if (_Part == static_cast<Part>(2))
			return PushBufferToField(u_done);
		break;
	case '%':
		return u_percent;
	case '?':
		return u_query;
	case '/':
		return State(_Part);
	default:
		if (IsPChar(c))
			return State(_Part);
		else
			return u_invalid;
	}
	return u_invalid;
}

// Pushes buffer to appropriate URI field when valid ending token indicates
// transition out of current (path/query) state to next or final state.
// `skip` is optional argument that defaults to TRUE.
State TargetRequestParser::PushBufferToField(State next, bool skip)
{
	if (_Part == static_cast<Part>(1))
	{
		size_t pos = _Buf.find_first_of("?");
		_Uri->_Path = _Buf.substr(0, pos);
	}
	else
		_Uri->_Query = _Buf;
	_Buf.clear();
	_SkipChar = skip;
	return next;
}

TargetRequestParser::TargetRequestParser() : HostStateParser(u_start, u_done),
											 _Uri(NULL),
											 _Part(static_cast<Part>(-1)) {}

TargetRequestParser::~TargetRequestParser() {}

size_t TargetRequestParser::Parse(Uri &uri, std::string const &input)
{
	_Uri = &uri;
	_Part = static_cast<Part>(1);
	return ParseString(input);
}

State TargetRequestParser::GetNextState(size_t index)
{
	static State (TargetRequestParser::*tab[])(char c) = {
		&TargetRequestParser::StartHandler,
		&TargetRequestParser::PathHandler,
		&TargetRequestParser::QueryHandler,
		&TargetRequestParser::PercentHandler,
		&TargetRequestParser::PercentDoneHandler,
		NULL};
	_SkipChar = false;
	return (this->*tab[_CurrentState])(_Input[index]);
}

void TargetRequestParser::CheckInvalidState() const
{
	if (_CurrentState == u_invalid)
		throw std::invalid_argument("Error: Invalid Request Target");
}

// check if uri > 8kb (limit for most web servers)
void TargetRequestParser::PreParseCheck()
{
	if (_Input.size() > 8192)
		throw std::invalid_argument("Error: Uri Too Long");
}

// check if there are more charecters after terminating token
void TargetRequestParser::AfterParserCheck()
{
	if (_CurrentState == u_done && _Index < _Input.size() - 1)
		throw std::invalid_argument("Error: Extra characters after terminating token in request target");
}
