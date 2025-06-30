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
struct Route
{
private:
    std::string path;
    std::string index;
    std::vector<std::string> methods;
    std::string root;
    std::string redirection;
    std::string upload_path;
    std::map<std::string, std::string> cgi_extensons;
    bool autoindex;

public:
    const Route &operator=(const Route &other);
    Route() : root(""), redirection(""), autoindex(false) {};
    Route(const Route &other);
    std::string getPath() const;
    std::string getIndex() const ;
    const std::vector<std::string> &getMethods() const;
    std::string getRoot() const;
    std::string getRedirection() const;
    bool getAutoindex() const;
    const std::map<std::string, std::string> &getCGI() const;
    std::string getUpload() const;
    std::string setPath(std::string path) ;
    std::string getCgiPath(const std::string& request_path) const;
    void setIndex(std::string index);
    void setMethods(std::vector<std::string> methods);
    void setRoot(std::string root);
    void setRedirection(std::string redirection);
    void setAutoindex(bool autoindex);
    void setCGI(std::map<std::string, std::string> table);
    void setUpload(std::string &path);
};

class Server
{
private:
    std::vector<int> port;
    // check if host is possible to be "localhost"
    std::string host;
    std::vector<std::string> server_name;
    std::map<std::string, Route> routes;
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
    void setRoutes(std::map<std::string, Route> routes);
    void setErrorPages(std::map<int, std::string> error_pages);
    void setMaxBodySize(int max_body_size);

    void set(std::string &key, TOMLValue &val);
    std::vector<int> &getPorts();
    std::string getHost();
    std::vector<std::string> &getServerName();
    std::map<std::string, Route> &getRoutes();
    std::map<int, std::string> &getErrorPages();
    std::vector<int> &getSocket();
    void setSocket(int socket);
    int getMaxBodySize();
    void printServer();
};

#endif

// std::map<std::string, route>
