#include "includes.hpp"


/*
	Author: BOUZID Hicham
	Description: handle multiple sockets
	Date: 2025-02-13
*/

void Multiple_connections()
{
	struct pollfd  *pollfds;
	nfds_t fds = 3;
	pollfds = (struct pollfd *)malloc(sizeof(struct pollfd) * 3);

	if (!pollfds)
		std::cerr << "Error: " << strerror(errno) << "\n";
	pollfds[0].fd = 3;
	pollfds[0].events = POLLIN;
	int ret = poll(&pollfds[0], fds, 5000);
	if (ret == -1)
		std::cout << "-----> " << strerror(errno) << '\n';
	else if (!ret)
		std::cout << "time out was accurred .\n";
	else
		std::cout << "The number of file descriptor with event: " << ret << "\n" ;
}
