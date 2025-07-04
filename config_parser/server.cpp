#include "server.hpp"
#include <algorithm>
#include "utilities.hpp"

const Route &Route::operator=(const Route &other)
{
    if (this != &other)
    {
        this->path = other.path;
        this->index = other.index;
        this->methods = other.methods;
        this->root = other.root;
        this->redirection = other.redirection;
        this->autoindex = other.autoindex;
        this->cgi_extensons = other.cgi_extensons;
        this->upload_path = other.upload_path;
    }
    return *this;
}

Route::Route(const Route &other)
{
    this->path = other.path;
    this->index = other.index;
    this->methods = other.methods;
    this->root = other.root;
    this->redirection = other.redirection;
    this->autoindex = other.autoindex;
    this->cgi_extensons = other.cgi_extensons;
    this->upload_path = other.upload_path;

}

std::string Route::getCgiPath(const std::string& request_path) const {
    std::string relative_path = request_path.substr(this->path.size());

    return this->root + "/" + relative_path;
}

std::string Route::getPath() const
{
    return this->path;
}

std::string Route::getIndex() const
{
    return this->index;
}

void Route::setCGI(std::map<std::string,std::string> table)
{
    this->cgi_extensons = table;

}

void Route::setUpload(std::string &path)
{
    this->upload_path = path;
}

std::string Route::getUpload() const
{
    return this->upload_path;
}
const std::map<std::string, std::string> & Route::getCGI() const
{
    return this->cgi_extensons;
}
const std::vector<std::string> &Route::getMethods() const
{
    return this->methods;
}

std::string Route::getRoot() const
{
    return this->root;
}

std::string Route::getRedirection() const
{
    return this->redirection;
}

bool Route::getAutoindex() const
{
    return this->autoindex;
}

std::string Route::setPath(std::string path)
{
    this->path = path;
    return this->path;
}

void Route::setIndex(std::string index)
{
    this->index = index;
}

void Route::setMethods(std::vector<std::string> methods)
{
    this->methods = methods;
}

void Route::setRoot(std::string root)
{
    this->root = root;
}

void Route::setRedirection(std::string redirection)
{
    this->redirection = redirection;
}

void Route::setAutoindex(bool autoindex)
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

std::map<std::string, Route> &Server::getRoutes()
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

void Server::setRoutes(std::map<std::string, Route> routes)
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
std::vector<int> &Server::getSocket(){
    return this->socket;
}

void Server::setSocket(int socket){
    this->socket.push_back(socket);
    // std::cout << this->socket.size() << "  " << socket << " ????????\n";
}

Server::Server(const Server &other) : webServ(other.webServ)
{
    this->host = other.host;
    this->port = other.port;
    this->server_name = other.server_name;
    this->routes = other.routes;
    this->error_pages = other.error_pages;
    this->max_body_size = other.max_body_size;
}

const Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        this->host = other.host;
        this->port = other.port;
        this->server_name = other.server_name;
        this->routes = other.routes;
        this->error_pages = other.error_pages;
        this->max_body_size = other.max_body_size;
    }
    return *this;
}



void Server::printServer()
{
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
    for (std::map<std::string, Route>::iterator it = this->routes.begin(); it != this->routes.end(); it++)
    {
        std::cout << " Path: " << it->second.getPath() << std::endl;
        std::cout << " Index: " << it->second.getIndex() << std::endl;
        std::cout << " Methods : " << std::endl;
        for (std::vector<std::string>::const_iterator it2 = it->second.getMethods().begin(); it2 != it->second.getMethods().end(); it2++)
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
