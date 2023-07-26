/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Root.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 18:01:12 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/08 18:31:26 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/Root.hpp"

Root::Root(std::string input)
{
	if (input.compare("") == 0)
		throw std::invalid_argument("Error: Missing Root Argument");
	size_t arg = CountArgs(input);
	if (arg != 1)
		throw std::invalid_argument("Error: Too Many Root Arguments");
}

Root::~Root() {}
