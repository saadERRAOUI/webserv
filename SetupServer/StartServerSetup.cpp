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
	int reuse = 1;
	memset(&address, 0, sizeof(sockaddr));
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	// address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(fd_sock, SOL_SOCKET, SO_REUSEPORT , &reuse, sizeof(int)))
		std::cerr << "setsockopt(SO_REUSPORT) failed" << "\n";
	std::cout << "-----> ::::: " << fd_sock << "\n";
	int Rt_Bind = bind(fd_sock, (sockaddr *)&address, sizeof(address));
	if (Rt_Bind == -1)
		std::cerr << "Error happen with : " << strerror(errno) << "\n";
	if (listen(fd_sock, 2) == -1)
		std::cerr << "can not make a socket passive to accept connection\n";
	std::cout << "our webserve is ready now ....\n";
	struct sockaddr_in client_addr;
	socklen_t len_client = sizeof(client_addr);
	int client_sock = accept(fd_sock, (sockaddr *)&client_addr, &len_client);
	if (client_sock == -1)
		std::cout << "Error cannot connect: " << strerror(errno) << '\n';
	char buffer[2024] = {0};
	read(client_sock, buffer, 2024);
	printf("%s", buffer);
	write(client_sock, "hello world\n", sizeof("hello world\n"));
	close(fd_sock);
	close(client_sock);
	close(address.sin_port);
}

// next step binding this socket with port /
// next step printing the content of  address struct and then move to under strund how bind work
int main ()
{
	setup();
}
