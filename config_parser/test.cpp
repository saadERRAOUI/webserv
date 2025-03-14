#include  "parser.hpp"
#include <iostream>
#include "Webserv.hpp"


int main()
{
    try {
        WebServ web("./config.toml");
        web.getServers()[0].printServer() ;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
 
       throw e;
    }

}