#include "includes.hpp"

Server::Server(int port, int flag){
	std::cout << "Severe object was create socket for: "
		<< (!flag ? "Server\n": "Client\n");
	this->fdsock = !flag ? SetUpServer(port) : port;
	this->port = port;
	this->host = std::string("");
	this->flag = flag;
}

Server::~Server(){
	close(fdsock);
	std::cout << "Servere with port: " << this->port << " was closed.\n";
}

int  Server::Getsockfd() const{
	return (this->fdsock);
}

int  Server::Getflag() const{
	return (this->flag);
}

/*
	Author: BOUZID Hicham
	Description: set up server and make it ready to reach connections
	Date: 2025-02-07
*/
void  ft_client(int fdsv, int epollfd, std::map<int, Server *> *sockets)
{
	std::cout << "hellooooo \n";
	struct sockaddr_in client;
	struct  epoll_event event;
	// Server * client_sock;

	socklen_t client_lenght = sizeof(client);
	int fd =  accept(fdsv, (sockaddr *)&client, &client_lenght);
	if (fd < 0)
		std::cerr << "Error on accept function: " << strerror(errno) << "\n";
	// client_sock = new Server(fd, -1);
	(*sockets)[fd] = new Server(fd, -1);;
	event.events = EPOLLOUT;
	event.data.fd =  fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event))
			std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
}

/*
	Author: BOUZID Hicham
	Description: add socketfd to epoll inctence
	Date: 2025-02-13
*/

void Sockets_manager(std::map<int , Server *> * sockets, int epollfd)
{
	int index;
	struct epoll_event *event  = (epoll_event *)malloc(sizeof(epoll_event) * sockets->size());
	struct epoll_event Queueevent[MAX_EPOLL_EVENT];
	int fds;
	// char buffer[1024] = {0};
	index = 0;
	for (std::map<int, Server *>::iterator  it = sockets->begin(); it != sockets->end(); it++ , index++)
	{
		event[index].events =  EPOLLIN;
		event[index].data.fd =  it->first;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, it->first, &event[index]))
			std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
	}
	for (;;)
	{
		fds = epoll_wait(epollfd, Queueevent, MAX_EPOLL_EVENT, -1);
		for (int i = 0; i < fds; i++){
				std::cout << "fd readd: " << (*sockets)[Queueevent[i].data.fd]->Getsockfd()  << '\zn';
			if ((*sockets)[Queueevent[i].data.fd]->Getsockfd() ==  0){
				ft_client(Queueevent[i].data.fd, epollfd, sockets);
				std::cout << "hello 1\n";
			}
			// else{
			// 	int fd_read = (*sockets)[Queueevent[i].data.fd]->Getsockfd();
			// 	int size = read(fd_read, buffer, 1024);
			// 	std::cout << "=======>  " << size << "\n";
			// 	std::cout << "fd readd: " << fd_read << "\n";
			// 	exit(0);
			// 	std::cout << std::string (buffer) << "\n";
			// 	// exit(0);
			// }
		}
		// for (int i = 0; i < fds; i++)
		// {
		// 	for (int j = 0; j < size; j++)
		// 	{	
		// 		if (Queueevent[i].data.fd == event[j].data.fd)
		// 		{
						
		// 		}
		// 	}
			// printf("Got an event %d for this fd: %d \n",
			// 	Queueevent[i].events, Queueevent[i].data.fd);
			//  read(Queueevent[i].data.fd, buffer, 1024);
			// std::cout << "-----> " << fds << '\n';
			// printf("%s\n", buffer);
		// }
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
