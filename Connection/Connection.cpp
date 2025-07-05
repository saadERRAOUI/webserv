#include "Connection.hpp"

Connection::~Connection(){
	// delete this->Request;
	// delete this->Response;
}

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
	event.events = EPOLLIN;
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
				s = &(*it);
				this->CGI = NULL;
				Request = NULL;
				Request = new HttpRequest();
				Response = NULL;
				timeout = get_current_time();
				path_optional = "";
				size = 0;
				fd_client = ft_client(FdServer, FdEpoll);
				done = false;
			}
		}
	}
}

Connection::Connection(){
	this->CGI = NULL;
	Request = NULL;
	s = NULL;
	fd_client = 0;
	size = 0;
	path_optional = "";
	done = false;
	// request = std::string("");
}

int Connection::Getfd(){
	return this->fd_client;
}


void Connection::SetHttpRequest(HttpRequest *Request)
{
	this->Request = Request;
}

HttpRequest &Connection::GetRequest(){
	return (*this->Request);
}

void		Connection::SetHttpResponse(HttpResponse *Response){
	this->Response =  Response;
}

HttpResponse& Connection::GetResponse(){
	return (*this->Response);
}

Server& Connection::Getserver(){
	return (*this->s);
}

void Connection::SetBool(bool f){
    std::cout << "[SetBool] called with value: " << f << std::endl;
    this->done = f;
}

void Connection::SetCGI(Cgi *cgi){
	this->CGI = cgi;
}

Cgi *Connection::getCGI(){
	return this->CGI;
}

bool Connection::GetBool(){
	return (this->done);
}

/*
	Author: BOUZID Hicham
	Description: add a uri redirection to a
		vector to avoid unfinit redirection
	Date: 2025-04-29
*/

bool Connection::SetRedirect(std::string URI){
	std::vector<std::string>::iterator it = find(redirect.begin(), redirect.end(), URI);
	if (it == redirect.end())
	{
		this->redirect.push_back(URI);
		return (true);
	}
	return (false);
}


void Connection::SetSize(int n){
	this->size = n;
}

int Connection::GetSize(){
	return (this->size);
}

/*
	Author: BOUZID Hicham
	Description: this function defernce
				the whole size of file from the readed
	Date: 2025-05-05
*/
void Connection::DefSize(int n){
	this->size -= n;
	if (this->size < BUFFER_SIZE){
		SetBool(true);
		file->close();
	}
}


void	Connection::Setfile(std::ifstream &file){
	this->file = &file;
}


std::ifstream *Connection::GetFile(){
	return file;
}

/*
	Author: BOUZID Hicham
	Description: change the settings associated with fd
	date: 2025-05-05
*/

void Connection::ChagenMode(int FdEpoll, int fd_client, int mood)
{
	struct epoll_event event;
	// std::cout << "pepe\n";
	event.events = mood;
	event.data.fd = fd_client;
	if (epoll_ctl(FdEpoll, EPOLL_CTL_MOD, fd_client, &event))
		std::cerr << "epoll ctl Error: " << strerror(errno) << '\n';
}

long Connection::GetTimeout() const
{
	return this->timeout;
}

void Connection::SetTimeout(long timeout)
{
	this->timeout = timeout;
}


/*
	Author: BOUZID Hicham
	Description: distructor of connection class
	Date: 2025-05-24
*/

// Connection::~Connection(){
// 	std::cout << "Distructor of connection class called.\n";
// 	delete this->file;
// 	delete this->Request;
// }
