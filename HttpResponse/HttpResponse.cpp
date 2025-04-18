#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int fd_client)
{
	std::cout << "need to remove just for check Response builder\n";
	this->fd_client = fd_client;
	offset_done = false;
	this->status_code[400] = std::string("Bad Request");
	this->status_code[403] = std::string("Forbidden");
	this->status_code[404] = std::string("Not Found");
	this->status_code[500] = std::string("Internal Server Error");
}

void ResponseBuilder(std::map<int, Connection> *map_connections, ){

}