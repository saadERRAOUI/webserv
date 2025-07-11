#include "includes.hpp"

/*
	Author: BOUZID Hicham
	Description: set up server and make it ready to reach connections
	Date: 2025-02-07
*/

// int ft_client(int fdsv, int epollfd)
// {
// 	struct sockaddr_in client;
// 	socklen_t client_line;
	

// }

int SetUpServer(int port)
{
	int fdsocket;
	struct sockaddr_in my_addr;
	int const enable = 1;

	fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fdsocket < 0)
		std::cerr << "Sock Error: " << strerror(errno) << "\n";
	memset(&my_addr, 0, sizeof(sockaddr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);				 // specifie the port to be used .
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // that mean any ip address can make a request to server .
	if (setsockopt(fdsocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
		std::cerr << "Error: " << strerror(errno) << "\n";
	if (bind(fdsocket, (sockaddr *)&my_addr, sizeof(my_addr)))
		std::cerr << "Binding error: " << strerror(errno) << "\n";
	if (listen(fdsocket, 2))
		std::cerr << "Error listen: " << strerror(errno) << "\n";

	// socklen_t len_client = sizeof(client_addr);
	// Multiple_connections();
	// while (1)
	// {
	// 	int rt = accept(fdsocket, (sockaddr *)&client_addr, &len_client);
	// 	char buffer[1024] = {0};
	// 	if (rt < 0)
	// 		std::cout << "Error accept: " << strerror(errno) << '\n';
	// 	read(rt, buffer, 1024);
		// std::cout << "=================\n";
		// std::cout << std::string(buffer);
		// std::cout << "=================\n";
	// 	const char *message =
	// 	    "HTTP/1.1 200 OK\r\n"
	// 	    "Content-Type: text/html; charset=UTF-8\r\n"
	// 	    "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
	// 	    "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
	// 	    "Content-Length: 1234\r\n"
	// 	    "\r\n"
	// 	    "<!doctype html>\n"
	// 	    "<html>\n"
	// 	    "<head><title>My Page</title></head>\n"
	// 	    "<body><h1>Welcome!</h1></body>\n"
	// 	    "</html>\n";
		
	// 	int n = write(rt, message, strlen(message));
		std::cout << "----> n: " << n << "\n";

	// }
	// std::cout << "out of loop\n";

	// close(fdsocket);
	// close(my_addr.sin_port);
	return (fdsocket);
}

int main()
{
	Server *tmp;
	std::map<int , Server *> sockets;
	for (int i= 8080; i < 8085; i++)
	{
		tmp = new Server(i, 0);
		sockets[tmp->Getsockfd()] = tmp;

	}
	int epollfd =  create_manager();
	Sockets_manager(&sockets, epollfd);
}
