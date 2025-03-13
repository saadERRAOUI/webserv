#include "server.hpp"












std::string route::getPath()
{
    return this->path;
}

std::string route::getIndex()
{
    return this->index;
}

std::vector<std::string> route::getMethods()
{
    return this->methods;
}

std::string route::getRoot()
{
    return this->root;
}

std::string route::getRedirection()
{
    return this->redirection;
}

bool route::getAutoindex()
{
    return this->autoindex;
}

std::string route::setPath(std::string path)
{
    this->path = path;
    return this->path;
}

void route::setIndex(std::string index)
{
    this->index = index;
}

void route::setMethods(std::vector<std::string> methods)
{
    this->methods = methods;
}

void route::setRoot(std::string root)
{
    this->root = root;
}

void route::setRedirection(std::string redirection)
{
    this->redirection = redirection;
}

void route::setAutoindex(bool autoindex)
{
    this->autoindex = autoindex;
}


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

