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

static bool HostName(Server *tmpServer, std::string name)
{
	std::vector<std::string>::iterator it = find(tmpServer->getServerName().begin(), tmpServer->getServerName().end(), name);
	if (it == tmpServer->getServerName().end())
		return (false);
	return (true);
	// tmpServer.getServerName();
}

void ResponseBuilder(Connection *Infos, bool flag){
	(void)Infos;
	(void)flag;
	std::string host = Infos->GetRequest().getHeaders()["Host"];
	std::cout << "==============> " << host << '\n';
	if (HostName(&Infos->Getserver(), host) == false){
		std::cerr << "server_name not found" << '\n';
	}
		// std::cout << "Hello World\n";
}
