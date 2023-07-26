/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Root.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/01 12:03:25 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/14 18:02:47 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROOT_HPP
#define ROOT_HPP

#include <string>
#include <stdexcept>
#include "./utils.hpp"

class Root {
	private:
		Root(){}
	public:
		Root(std::string input);
		~Root();

};

#endif
