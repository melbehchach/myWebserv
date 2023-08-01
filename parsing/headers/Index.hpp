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
