#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int fd_client)
{
	// std::cout << "need to remove just for check Response builder\n";
	this->fd_client = fd_client;
	offset = 0;
	this->status_code[200] = std::string("OK");
	this->status_code[301] = std::string("Moved Permanently");
	this->status_code[400] = std::string("Bad Request");
	this->status_code[403] = std::string("Forbidden");
	this->status_code[404] = std::string("Not Found");
	this->status_code[405] = std::string("Method Not Allowed");
	this->status_code[413] = std::string("Payload Too Large");
	this->status_code[500] = std::string("Internal Server Error");
}


void ResponseBuilder(Connection *Infos){

	std::string host = Infos->GetRequest().getHeaders()["Host"];
	Server *TmpServer =  &Infos->Getserver();

	if (HostName(&Infos->Getserver(), host) == false){
		write (Infos->Getfd(), ErrorBuilder(Infos, TmpServer, 400).c_str(), strlen(ErrorBuilder(Infos, TmpServer, 400).c_str()));
		Infos->SetBool(true);
		return ;
	}
	else if (Infos->GetRequest().getIsCGI())
	{
		std::cout << "im in";
			if (!Infos->getCGI())
			{
				std::cout << "first time, never again\n";
				Route &matchedRoute = Infos->Getserver().getRoutes()[MatchRoutes(Infos->Getserver().getRoutes(), Infos->GetRequest())];
				Cgi *cgi = new Cgi(matchedRoute, Infos->GetRequest());
				cgi->env_set_up();
				cgi->execute();
				std::cout << cgi->getResponse();
				Infos->SetCGI(cgi);
				std::exit(0);
			}	

	}
	else if (Infos->GetRequest().getMethod() == "GET"){
		std::cout << "Client requested to : " << Infos->GetRequest().getRequestURI() << '\n';

		std::string tmpstring = GetMethod(Infos);
		write (Infos->Getfd(), tmpstring.c_str(), strlen(tmpstring.c_str()));
		return ;
	}
	// else if (Infos->GetRequest().getMethod() == "POST"){

	// }
	else if (Infos->GetRequest().getMethod() == "DELETE"){

	}
}

/*
	Author: BOUZID Hicham
	Description: check for all connections
				that found in our map connecions if done
	Date: 2025-04-23
*/

void MonitorConnection(std::map<int, Connection> *Connections,int epollFd){
	std::map<int , Connection>::iterator it= Connections->begin();
	struct  epoll_event event;

	for (; it != Connections->end(); it++){
		if (it->second.GetBool() == true)
		{
			if (epoll_ctl(epollFd, EPOLL_CTL_DEL, it->first, &event))
					std::cerr << "EPOLL_CTL_DEL: " << strerror(errno) << '\n';
			close(it->first);
			break;
		}
	}
	if (it != Connections->end())
		Connections->erase(it->first);
}

std::string HttpResponse::GetStatusCode(int code_number){
	return status_code[code_number];
}
