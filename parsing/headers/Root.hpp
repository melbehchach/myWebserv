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
