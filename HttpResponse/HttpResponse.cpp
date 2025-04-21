#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int fd_client)
{
	// std::cout << "need to remove just for check Response builder\n";
	this->fd_client = fd_client;
	offset_done = false;
	this->status_code[400] = std::string("Bad Request");
	this->status_code[403] = std::string("Forbidden");
	this->status_code[404] = std::string("Not Found");
	this->status_code[500] = std::string("Internal Server Error");
}

/*
	Author: BOUZID Hicham
	Description: check if the server name that we recieved
				found on our config file or not
	Date: 2025-04-21
*/
static bool HostName(Server *tmpServer, std::string name)
{
	std::cout << "the name of server is : " << name << '\n';
	std::cout << "the first server name is: " << *(tmpServer->getServerName().begin()) << '\n';
	std::vector<std::string>::iterator it = find(tmpServer->getServerName().begin(), tmpServer->getServerName().end(), name);
	if (it == tmpServer->getServerName().end())
		return (false);
	return (true);
}

/*

*/

void ErrorBuilder(Connection *Infos, Server *tmpServer){
	std::s
}

void ResponseBuilder(Connection *Infos, bool flag){
	(void)Infos;
	(void)flag;
	std::string host = Infos->GetRequest().getHeaders()["Host"];
	std::cout << "==============> " << host << '\n';
	Server *TmpServer =  &Infos->Getserver();
	// HostName(TmpServer, host);
	if (HostName(&Infos->Getserver(), host) == false){
		std::cerr << "server_name not found" << '\n';
	}
		// std::cout << "Hello World\n";
}