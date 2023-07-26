/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReturnDir.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/01 12:03:28 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/14 18:00:38 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RETURNDIR_HPP
#define RETURNDIR_HPP

#include <string>
#include "./utils.hpp"

class ReturnDir {

	private:
		bool _Is_Set;
		int _Code;
		std::string _Url;


		bool ValidReturnCode(size_t code);
		void FillReturn(std::string input);
		void SetReturnCode(std::string key) ;
		void SetVariables(std::string key, size_t *set);

	public:
		ReturnDir() ;
		ReturnDir(std::string input);
		ReturnDir(ReturnDir const &obj);
		ReturnDir &operator=(const ReturnDir &obj);
		~ReturnDir() ;

		int GetCode() const ;
		std::string GetUrl()const;
		bool IsSet() const;

};

#endif
