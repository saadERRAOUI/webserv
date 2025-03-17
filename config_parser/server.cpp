#include "./../Includes/server.hpp"
#include "./../Includes/utilities.hpp"

const route &route::operator=(const route &other)
{
    if (this != &other)
    {
        this->path = other.path;
        this->index = other.index;
        this->methods = other.methods;
        this->root = other.root;
        this->redirection = other.redirection;
        this->autoindex = other.autoindex;
    }
    return *this;
}

route::route(const route &other)
{
    this->path = other.path;
    this->index = other.index;
    this->methods = other.methods;
    this->root = other.root;
    this->redirection = other.redirection;
    this->autoindex = other.autoindex;
}


std::string route::getPath()
{
    return this->path;
}

std::string route::getIndex()
{
    return this->index;
}

std::vector<std::string> &route::getMethods()
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


std::vector<int> &Server::getPorts()
{
    return this->port;
}

std::string Server::getHost()
{
    return this->host;
}

std::vector<std::string> &Server::getServerName()
{
    return this->server_name;
}

std::map<std::string, route> &Server::getRoutes()
{
    return this->routes;
}

std::map<int, std::string> &Server::getErrorPages()
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

Server::Server(const Server &other) : webServ(other.webServ)
{
    this->port = other.port;
    this->host = other.host;
    this->server_name = other.server_name;
    this->routes = other.routes;
    this->error_pages = other.error_pages;
    this->max_body_size = other.max_body_size;
}

const Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        this->port = other.port;
        this->host = other.host;
        this->server_name = other.server_name;
        this->routes = other.routes;
        this->error_pages = other.error_pages;
        this->max_body_size = other.max_body_size;
    }
    return *this;
}



void Server::printServer()
{
    std::cout << "Host : " <<this->host << std::endl;
    std::cout << "Server name : ";
    for (std::vector<std::string>::iterator it = this->server_name.begin(); it != this->server_name.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "Ports : ";
    for (std::vector<int>::iterator it = this->port.begin(); it != this->port.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "Max body size : ";
    std::cout << this->max_body_size << std::endl;
    std::cout << "Routes : " << std::endl;
    for (std::map<std::string, route>::iterator it = this->routes.begin(); it != this->routes.end(); it++)
    {
        std::cout << " Path: " << it->second.getPath() << std::endl;
        std::cout << " Index: " << it->second.getIndex() << std::endl;
        std::cout << " Methods : " << std::endl;
        for (std::vector<std::string>::iterator it2 = it->second.getMethods().begin(); it2 != it->second.getMethods().end(); it2++)
            std::cout << "          " <<*it2 << " ";
        std::cout << std::endl;
        std::cout << " Root : "  << it->second.getRoot() << std::endl;
        std::cout <<  " Redirection" << it->second.getRedirection() << std::endl;
        std::cout << " Auto index: " << it->second.getAutoindex() << std::endl;
    }
    std::cout << "Error pages" << std::endl;
    for (std::map<int, std::string>::iterator it = this->error_pages.begin(); it != this->error_pages.end(); it++)
    {
        std::cout <<  " Error code : " <<it->first << std::endl;
        std::cout << " Path : " << it->second << std::endl;
    }

}

void Server::set(std::string &key, TOMLValue &val)
{
    if (key == "port" && val.type == TOMLValue::ARRAY)
    {
        transform(val.array->begin(), val.array->end(), std::back_insert_iterator<std::vector<int> >(this->port),parse_positive_int);
    }
    else if (key == "host" && val.type == TOMLValue::SINGLE)
        this->setHost(*val.single);
    else if (key == "server_name" && val.type == TOMLValue::ARRAY)
        this->setServerName(std::vector<std::string>(val.array->begin(), val.array->end()));
    else if (key == "max_body_size" && val.type == TOMLValue::SINGLE)
        this->setMaxBodySize(parse_positive_int(*val.single));
    else
        throw std::invalid_argument("Invalid key/value");
}

