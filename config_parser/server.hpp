#ifndef SERVER_H

#define SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "parser.hpp"
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
public :
    std::string getPath();
    std::string getIndex();
    std::vector<std::string> getMethods();
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
    int max_body_size;

public:
    WebServ &webServ;
    Server(WebServ &webServ) : webServ(webServ) {};
    void setPort(std::vector<int> port);
    void setHost(std::string host);
    void setServerName(std::vector<std::string> server_name);
    void setRoutes(std::map<std::string, route> routes);
    void setErrorPages(std::map<int, std::string> error_pages);
    void setMaxBodySize(int max_body_size);

    void set(std::string &key, TOMLValue &val);
    std::vector<int> getPorts();
    std::string getHost();
    std::vector<std::string> getServerName();
    std::map<std::string, route> getRoutes();
    std::map<int, std::string> getErrorPages();
    int getMaxBodySize();
    void printServer();
};

#endif

// std::map<std::string, route>
