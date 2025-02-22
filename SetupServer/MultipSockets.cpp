#include "includes.hpp"


/*
	Author: BOUZID Hicham
	Description: add socketfd to epoll inctence
	Date: 2025-02-13
*/

void Sockets_manager(int sockfd, int epollfd)
{
	struct epoll_event event;
	struct epoll_event Queueevent[MAX_EPOLL_EVENT];
	char buffer[1024] = {0};
	int fds;

	event.events = EPOLLIN;
	event.data.fd = sockfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event))
		std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';

	// std::cout << "-----> " << event.events << "\n";
	for (;;)
	{
		fds = epoll_wait(epollfd, Queueevent, MAX_EPOLL_EVENT, -1);
	// std::cout << "-----> " << Queueevent[0].data.fd << "\n";
		if(fds != 1)
		{
			std::cout << "break----> " << fds << "\n";
		} 
		for (int i = 0; i < fds; i++)
		{
			if (Queueevent[i].data.fd == event.data.fd)
			{
				std::cout << "-----> " << Queueevent[i].data.fd << "\n";
				std::cout << "-----> " << sockfd << "\n";
				std::cout << "hellor we get a new request \n";
				exit(0);
			}
			// printf("Got an event %d for this fd: %d \n",
			// 	Queueevent[i].events, Queueevent[i].data.fd);
			//  read(Queueevent[i].data.fd, buffer, 1024);
			// std::cout << "-----> " << fds << '\n';
			// printf("%s\n", buffer);
		}
	}
}

/*
	Author: BOUZID hicham
	Description: craet epoll inctence to manange multiple sockets
	Date: 2025-02-18
*/

int create_manager()
{
	int EpollFd =    epoll_create(10);
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
