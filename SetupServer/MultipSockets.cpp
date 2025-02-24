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

	socklen_t client_lenght = sizeof(client);
	int fd =  accept(fdsv, (sockaddr *)&client, &client_lenght);
	if (fd < 0)
		std::cerr << "Error on accept function: " << strerror(errno) << "\n";
	(*sockets)[fd] = new Server(fd, -1);;
	event.events = EPOLLIN;
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
	struct epoll_event event;
	struct epoll_event Queueevent[MAX_EPOLL_EVENT];
	int fds;
	for (std::map<int, Server *>::iterator  it = sockets->begin(); it != sockets->end(); it++)
	{
		event.events =  EPOLLIN;
		event.data.fd =  it->first;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, it->first, &event))
			std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
	}
	for (;;)
	{
		char buffer[1024] = {0};
		fds = epoll_wait(epollfd, Queueevent, MAX_EPOLL_EVENT, -1);
		for (int i = 0; i < fds; i++){
			std::cout << "fd readd: " << (*sockets)[Queueevent[i].data.fd]->Getsockfd()  << " number of event: " << fds << 
				'\n';
			if ((*sockets)[Queueevent[i].data.fd]->Getflag() ==  0){
				ft_client(Queueevent[i].data.fd, epollfd, sockets);
			}
			else{

				int fd_read = (*sockets)[Queueevent[i].data.fd]->Getsockfd();
				int size = read(fd_read, buffer, 1024);
				std::cout << "=======>  " << size << " ============> " << fd_read << "\n";
				std::cout << std::string (buffer) << "\n";
				const char *message =
				    "HTTP/1.1 200 OK\r\n"
				    "Content-Type: text/html; charset=UTF-8\r\n"
				    "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
				    "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
				    "Content-Length: 1234\r\n"
				    "\r\n"
				    "<!doctype html>\n"
				    "<html>\n"
				    "<head><title>My Page</title></head>\n"
				    "<body><h1>Welcome!</h1></body>\n"
				    "</html>\n";
				write(fd_read, message, strlen(message));
				// event.events = EPOLLOUT;
				// event.data.fd =  fd_read;
				if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd_read, NULL))
						std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
				close(fd_read);
			}
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
	return (EpollFd);
}
