#pragma once
#include "../HttpRequest/HttpRequest.hpp"
#include "../Connection/Connection.hpp"



/*
	Author: BOUZID Hicham
	Description:  build response depending
					on the request parsing
	Date: 2025-04-15
*/
// class HttpRequest ;
class Connection;
class HttpResponse
{
	std::map<int, std::string> status_code;
	bool offset_done;
	int fd_client;
	public:
	HttpResponse(int fd_client);

	std::string			GetStatusCode(int);
};

void ResponseBuilder(Connection *, bool);
std::string tostring(int number);
std::string ErrorBuilder(Connection *Infos, Server *tmpServer, int code);
bool HostName(Server *tmpServer, std::string name);
std::string OpenFile(std::string PathFile, int status);
void MonitorConnection(std::map<int, Connection> *Connections, int epollFd);
