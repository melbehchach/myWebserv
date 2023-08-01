#ifndef CHECKMETHODS_HPP
#define CHECKMETHODS_HPP

#include <string>
#include <vector>
#include <exception>
#include "./utils.hpp"


class CheckMethods {

	public:
		CheckMethods();

		CheckMethods(std::string str);
		~CheckMethods();
		CheckMethods &operator=(const CheckMethods &obj);
		CheckMethods(const CheckMethods &obj);

		bool GetGET() const;
		bool GetPost() const;
		bool GetDelete() const;
		std::vector<std::string> GetMethods() const;


	private:
		bool _Get;
		bool _Post;
		bool _Delete;
		std::vector<std::string> _Methods;

		void ValidateMethods();

};





#endif
