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
