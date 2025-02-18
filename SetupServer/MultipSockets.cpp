#include "includes.hpp"


/*
	Author: BOUZID Hicham
	Description: add socketfd to epoll inctence
	Date: 2025-02-13
*/

void Sockets_manager(int sockfd, int epollfd)
{
	struct epoll_event event;
	int fds;
	event.events = EPOLLIN;
	event.data.fd = sockfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event))
		std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
	fds = epoll_wait(epollfd, &event, 10, -1);
	for (;;)
	{
		if (event.events && EPOLLIN)
			std::cout << "----------> " << strerror(errno) << '\n';
	}
}

/*
	Author: BOUZID hicham
	Description: craet epoll inctence to manange multiple sockets
	Date: 2025-02-18
*/

int create_manager()
{
	int EpollFd =    epoll_create1(EPOLL_CLOEXEC);
	if (EpollFd < 0)
	{
		std::cerr << "Epoll problem: " << strerror(errno) << '\n';
		exit(EXIT_FAILURE);
	}
	std::cout << "epoll inctence was created .\n";
	// std::cout << "---> " << EpollFd << '\n';
	// close(EpollFd);
	return (EpollFd);
}
