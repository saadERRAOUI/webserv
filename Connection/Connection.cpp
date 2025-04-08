#include "Connection.hpp"

/*
	Author: BOUZID Hicham
	Description: function get the current
				time since  January 1, 1970
				copied from philosophers project
	Date: 2025-04-08
*/
long	get_current_time(void)
{
	struct timeval	time;
	long			nbr;

	if (gettimeofday(&time, NULL))
	{
		// ft_putstr("error was happen ind gettimeofday\n", 2);
		std::cerr << "Error gettimeofday(): " << strerror(errno);
		return (-1);
	}
	nbr = time.tv_sec * 1000000;
	nbr += time.tv_usec;
	return (nbr);
}

/*
	Author: BOUZID Hicham
	Description: accept connection and assign fd_client to epoll
	Date: 2025-04-07
*/

int ft_client(int FdServer, int FdEpoll){
	struct sockaddr_in client;
	struct  epoll_event event;

	socklen_t client_lenght = sizeof(client);
	int fd_client =  accept(FdServer, (sockaddr *)&client, &client_lenght);
	if (fd_client < 0)
		std::cerr << "Error on accept function: " << strerror(errno) << "\n";
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd =  fd_client;
	if (epoll_ctl(FdEpoll, EPOLL_CTL_ADD, fd_client, &event))
			std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
	return (fd_client);
}


Connection::Connection(int FdServer, int FdEpoll,WebServ *MainObject)
{
	// HttpRequest *Request;
	// Server *s;
	// int fd_client;
	for (std::vector<Server>::iterator it = MainObject->getServers()->begin(); it != MainObject->getServers()->end(); it++)
	{
		for (std::vector<int>::iterator it_sock = it->getSocket().begin(); it_sock != it->getSocket().end(); it_sock++)
		{
			if (*it_sock == FdServer)
			{
				s = (Server *)&it;
				Request = NULL;
				timeout = get_current_time();
				fd_client = ft_client(FdServer, FdEpoll);
			}
		}
	}
}

Connection::Connection(){
	Request = NULL;
	s = NULL;
	fd_client = 0;
}

int Connection::Getfd(){
	return this->fd_client;
}