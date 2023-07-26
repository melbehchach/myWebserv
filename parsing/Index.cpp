/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Index.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/31 14:48:56 by aabdou            #+#    #+#             */
/*   Updated: 2022/12/08 18:31:13 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./headers/Index.hpp"

Index::Index(std::string val)
{
	if (val.compare("") == 0)
		throw std::invalid_argument("Error: Bad Index");
	_Index = ToVectorOfStrings(val);
}

Index::~Index() {}

std::vector<std::string> Index::GetIndex() const
{
	return _Index;
}

void Index::PrintIndex()
{
	for (size_t i = 0; i < this->_Index.size(); i++)
	{
		std::cout << _Index.at(i) << std::endl;
	}
}
