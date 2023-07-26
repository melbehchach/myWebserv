/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HostStateParser.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 22:18:20 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/17 18:09:31 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOSTSTATEPARSER_HPP
#define HOSTSTATEPARSER_HPP

#include "./utils.hpp"


template<class T>
class HostStateParser {
	/*
		abstract class that defines the state parsing as a template method
		T refers to the type of states
	*/

	protected:

		T _StartState;
		T _EndState;
		T _CurrentState;
		std::string _Buf; // keep track of the parsed input
		std::string _Input; // original input
		size_t _Index; // number of chars read
		bool _SkipChar; // for skipping EOF and/or whitespace


		virtual bool NotDone(size_t pos) const {
			if (pos <= _Input.size() && _CurrentState != _EndState)
				return true;
			return false;
		}

		virtual void IncrementConter() {
			_Index++;
		}

		virtual void UpdateBuffer(size_t index) {
			if (_SkipChar == false)
				_Buf += _Input[index];
		}

		virtual void InitParser(std::string const & input) {
			_Input = input;
			_SkipChar = false;
			_Index = 0;
		}

		virtual bool CheckDoneState() {
			return _CurrentState == _EndState;
		}

		virtual void CheckInvalidState() const = 0;
		virtual T GetNextState(size_t index) = 0;

		// hook functions
		virtual void PreParseCheck() {}
		virtual void AfterParserCheck() {}

	public:

		HostStateParser(T StartState, T EndState) :
		_StartState(StartState),
		_EndState(EndState),
		_CurrentState(StartState),
		_SkipChar(false) {}

		size_t ParseString(std::string const &str) {
			InitParser(str);
			PreParseCheck();
			while (NotDone(_Index)) {
				_CurrentState = GetNextState(_Index);
				UpdateBuffer(_Index);
				CheckInvalidState();
				if (CheckDoneState())
					break;
				IncrementConter();
			}
			AfterParserCheck();
			return _Index;
		}

		bool IsDone() {  // for child class to check if parsing is done
			return CheckDoneState();
		}



};

#endif
