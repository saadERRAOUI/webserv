#pragma once
#include "../HttpRequest/HttpRequest.hpp"
#include "../Connection/Connection.hpp"



/*
	Author: BOUZID Hicham
	Description:  build response depending
					on the request parsing
	Date: 2025-04-15
*/
class Connection;

class HttpResponse
{
	std::map<int, std::string> status_code;
	int offset;
	int fd_client;
	public:
	HttpResponse(int fd_client);

	std::string			GetStatusCode(int);
};

void			ResponseBuilder(Connection *);
std::string 	tostring(int number);
std::string 	ErrorBuilder(Connection *Infos, Server *tmpServer, int code);
bool 			HostName(Server *tmpServer, std::string name);
std::string 	OpenFile(std::string PathFile, bool status, Connection *Infos);
void 			MonitorConnection(std::map<int, Connection> *Connections, int epollFd);
std::string 	GetMethod(Connection *Infos);
std::string 	chose_one(std::string a, std::string b);
long	 		GetLenght(std::string PathFile);
std::string 	ft_Get(Connection *Infos, std::string URI, std::string route, int code);