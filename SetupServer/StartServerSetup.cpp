#include "includes.hpp"

/*
	Author: BOUZID Hicham
	Description: set up server and make it ready to reach connections
	Date: 2025-02-07
*/

void SetUpServer()
{
	int fdsocket;
	struct sockaddr_in my_addr;
	int const enable = 1;

	fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	std::cout << fdsocket << " it's fd socket \n";
	if (fdsocket < 0)
		std::cerr << "Sock Error: " << strerror(errno) << "\n";
	memset(&my_addr, 0, sizeof(sockaddr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(8080); // specifie the port to be used .
	my_addr.sin_addr.s_addr  = htonl(INADDR_ANY); // that mean any ip address can make a request to server .
	if (setsockopt(fdsocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
		std::cerr << "Error: " << strerror(errno) << "\n";
	if (bind(fdsocket, (sockaddr *)&my_addr, sizeof(my_addr)))
		std::cerr <<  "Binding error: " << strerror(errno) << "\n";
	if (listen(fdsocket, 2))
		std::cerr << "Error listen: " << strerror(errno) << "\n";
	socklen_t len_client = sizeof(my_addr);
	while (1)
	{
	int rt = accept(fdsocket, (sockaddr *)&my_addr, &len_client);
	char buffer[1024] = {0};
		if (rt < 0)
			std::cout << "Error accept: " << strerror(errno) << '\n';
		read(rt, buffer, 1024);
		std::cout << std::string(buffer) << "\n";
		const char *message = "hello world\n";
		int n = write(rt, message, strlen(message));
		std::cout << "Error: " << strerror(errno) << "\n";
		std::cout << "----> n: " << n << "\n";
	}
	std::cout << "out of loop\n";

	close(fdsocket);
	close(my_addr.sin_port);
}

int main()
{
	SetUpServer();
}
