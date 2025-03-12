#include "server.hpp"
















std::vector<int> Server::getPorts()
{
    return this->port;
}

std::string Server::getHost()
{
    return this->host;
}

std::vector<std::string> Server::getServerName()
{
    return this->server_name;
}

std::map<std::string, route> Server::getRoutes()
{
    return this->routes;
}

std::map<int, std::string> Server::getErrorPages()
{
    return this->error_pages;
}

int Server::getMaxBodySize()
{
    return this->max_body_size;
}


void Server::setPort(std::vector<int> port)
{
    this->port = port;
}

void Server::setHost(std::string host)
{
    this->host = host;
}

void Server::setServerName(std::vector<std::string> server_name)
{
    this->server_name = server_name;
}

void Server::setRoutes(std::map<std::string, route> routes)
{
    this->routes = routes;
}

void Server::setErrorPages(std::map<int, std::string> error_pages)
{
    this->error_pages = error_pages;
}

void Server::setMaxBodySize(int max_body_size)
{
    this->max_body_size = max_body_size;
}

