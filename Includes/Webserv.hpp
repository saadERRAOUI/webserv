#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// Include std libraries
// Add Macros
// Add Constants

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <algorithm>
#include <map>
// #include <stdlib.h>
enum HttpRequestState
{
	HTTP_METHOD,
	HTTP_REQUEST_URI,
	HTTP_VERSION,
	HTTP_HEADERS,
	HTTP_BODY,
	HTTP_COMPLETE,
	HTTP_ERROR
};

#include <vector>
#include "server.hpp"
#include "parser.hpp"
class WebServ
{
private:
	std::vector<Server> servers;
	std::string config_file;
	std::map<int, std::string> error_pages;
	int default_max_body_size;
	ConfigParser parser;

public:
	WebServ(std::string config_file);
	Server parseServer(Section &section);
	route parseRoute(Section &section);
	std::vector<Server> &getServers();
	int getDefaultMaxBodySize();
	std::map<int, std::string> &getErrorPages();
};

#endif
