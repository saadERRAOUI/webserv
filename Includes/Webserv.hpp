#pragma once



#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "server.hpp"
#include "parser.hpp"
class WebServ 
{
    private :
    std::vector <Server> servers;
    std::string config_file;
    std::map<int, std::string> error_pages;
    int default_max_body_size; 
    ConfigParser parser;
    public:
        WebServ(std::string config_file);
        Server parseServer(Section &section);
        std::vector<Server> getServers();
};