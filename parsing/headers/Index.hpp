/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Index.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabdou <aabdou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/31 14:48:53 by aabdou            #+#    #+#             */
/*   Updated: 2022/11/14 18:54:54 by aabdou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INDEX_HPP
#define INDEX_HPP

#include <vector>
#include <string>
#include "utils.hpp"

class Index {
	private:
		std::vector<std::string> _Index;
		Index(){};
	public:
		Index(std::string value);
		~Index();
		std::vector<std::string> GetIndex() const;
		void PrintIndex();

};

#endif
