#pragma once

//Include std libraries
//Add Macros
//Add Constants

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "./../Includes/server.hpp"
#include "./../Includes/parser.hpp"

#include <vector>

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
        std::vector<Server> getServers();
};
