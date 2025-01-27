#include "includes.hpp"

/*
	Author		: Hicham BOUZID
	Date		: 2025-01-27
	Description : initial function that set up Server.
*/
void setup()
{
	int fd_sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in address;
	memset(&address, 0, sizeof(sockaddr));
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	// address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::cout << "-----> ::::: " << fd_sock << "\n";
	int Rt_Bind = bind(fd_sock, (sockaddr *)&address, sizeof(address));
	if (Rt_Bind == -1)
		std::cerr << "Error happen with : " << strerror(errno) << "\n";
	std::cout << "sin family: " << address.sin_family << "\n";
	std::cout << "sin port: " << address.sin_port << "\n";
	std::cout << "sin addr: " << inet_ntoa(address.sin_addr) << "\n";
	std::cout << "-------> ::::: " <<  Rt_Bind << "\n";
}

// next step binding this socket with port /
// next step printing the content of  address struct and then move to under strund how bind work
int main ()
{
	setup();
}
