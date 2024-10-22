#ifndef LISTEN_HPP
#define LISTEN_HPP

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>

class Listen {
	private:
		std::string _Listen;
		std::string _PortNumber;
		std::string _Ip;

		Listen();
		Listen(const Listen &obj);
		Listen &operator=(const Listen &obj);

	public:
		Listen(std::string str);
		void SplitPortIp();
		void CheckPortNb(std::string port);
		bool CheckValidAddress(std::string ip);
		void CheckIpAddress(std::string ip);
		std::string GetPortNb();
		std::string GetIpNb();


};


#endif
