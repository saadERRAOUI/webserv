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
	Author: BOUZID Hicham
	Description: open file and return lenght
	Date: 2025-05-21
*/

std::string OpenFile(std::string PathFile, int status)
{
	std::ifstream fd(PathFile.c_str());
	std::string line, rt;
	(void)status;
	std::cout << "_________________> " << PathFile << "\n";
	fd.open(PathFile.c_str(), std::ifstream::in);
	if (!fd)
	{
		std::cerr << "Error file : " << strerror(errno) << '\n';
		return (std::string(""));
	}
	std::getline(fd, line);
	rt = line;
	while (std::getline(fd, line)){
		rt += line;
	}
	std::cout << "================================\n";
	std::cout << rt ;
	std::cout << "================================\n";
	fd.close();
	return (rt);
}

/*
	Author: BOUZID Hicham
	Description: build a response for error stats
				and serve the default error pages
				of ours error pages.
	Date: 2025-04-21
*/
void ErrorBuilder(Connection *Infos, Server *tmpServer, int code){
	std::string response = Infos->GetRequest().getVersion();
	std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();


	response += " 400 ";
	response += Infos->GetResponse().GetStatusCode(code);
	response += "\n";
	for (std::map<std::string, std::string>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
	{
		response += it->first;
		response += ":";
		response += it->second;
		response += "\n";
	}
	response += "Content-Length: " ;
	// tmpServer->getErrorPages()[code];
	std::string rt = OpenFile(std::string("./../www/html/ErrorPages/") + tmpServer->getErrorPages()[code], code);
	// function open a file and then return the lenght

}

void ResponseBuilder(Connection *Infos, bool flag){
	(void)Infos;
	(void)flag;
	std::string host = Infos->GetRequest().getHeaders()["Host"];
	std::cout << "==============> " << host << '\n';
	Server *TmpServer =  &Infos->Getserver();
	// HostName(TmpServer, host);
	if (HostName(&Infos->Getserver(), host) == false){
		ErrorBuilder(Infos, TmpServer, 400);
		std::cerr << "server_name not found" << '\n';
	}
		// std::cout << "Hello World\n";
}

std::string HttpResponse::GetStatusCode(int code_number){
	return status_code[code_number];
}
