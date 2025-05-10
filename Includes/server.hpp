#ifndef SERVER_H
#define SERVER_H

#include "parser.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>
#include <stdio.h>

#define MAX_EPOLL_EVENT 64
class WebServ;
struct route
{
private:
    std::string path;
    std::string index;
    std::vector<std::string> methods;
    std::string root;
    std::string redirection;
    bool autoindex;

public:
    const route &operator=(const route &other);
    route() : root(""), redirection(""), autoindex(false) {};
    route(const route &other);
    std::string getPath();
    std::string getIndex();
    std::vector<std::string> &getMethods();
    std::string getRoot();
    std::string getRedirection();
    bool getAutoindex();
    std::string setPath(std::string path);
    void setIndex(std::string index);
    void setMethods(std::vector<std::string> methods);
    void setRoot(std::string root);
    void setRedirection(std::string redirection);
    void setAutoindex(bool autoindex);
};

class Server
{
private:
    std::vector<int> port;
    // check if host is possible to be "localhost"
    std::string host;
    std::vector<std::string> server_name;
    std::map<std::string, route> routes;
    std::map<int, std::string> error_pages;
    std::vector<int> socket;
    int max_body_size;

public:
    WebServ &webServ;
    Server(WebServ &webServ) : max_body_size(-1), webServ(webServ) {};
    Server(const Server &other);
    const Server &operator=(const Server &other);
    void setPort(std::vector<int> port);
    void setHost(std::string host);
    void setServerName(std::vector<std::string> server_name);
    void setRoutes(std::map<std::string, route> routes);
    void setErrorPages(std::map<int, std::string> error_pages);
    void setMaxBodySize(int max_body_size);

    void set(std::string &key, TOMLValue &val);
    std::vector<int> &getPorts();
    std::string getHost();
    std::vector<std::string> &getServerName();
    std::map<std::string, route> &getRoutes();
    std::map<int, std::string> &getErrorPages();
    std::vector<int> &getSocket();
    void setSocket(int socket);
    int getMaxBodySize();
    void printServer();
};

#endif

// std::map<std::string, route>
