/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientMaxBodySize.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 15:30:19 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/11 20:22:32 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/ClientMaxBodySize.hpp"

ClientMaxBodySize::ClientMaxBodySize() : _val(1) {}

ClientMaxBodySize::ClientMaxBodySize(std::string val)
{
	size_t i = 0;
	size_t max = 0;
	size_t tmp;
	if (val.compare("") == 0)
		throw std::invalid_argument("Error: Missing Client Max Body Size");
	while (val[i])
	{
		if (std::isdigit(val[i]) == 0 && val[i] != 'm' && val[i] != 'M')
			throw std::invalid_argument("Error: invalid Client Max Body Size");
		if (std::isdigit(val[i]) == 1 && max == 1)
			throw std::invalid_argument("Error: invalid Client Max Body Size");
		else if (val[i] == 'm' || val[i] == 'M')
		{
			if (max == 1)
				throw std::invalid_argument("Error: invalid Client Max Body Size");
			max = 1;
		}
		i++;
	}
	tmp = StringToSize_T(val.substr(0, i));
	if (tmp > 2147483647)
		throw std::invalid_argument("Error: Max Body Size Too Large");
	_val = tmp;
}

ClientMaxBodySize::~ClientMaxBodySize() {}

size_t ClientMaxBodySize::GetVal() const
{
	return _val;
}
