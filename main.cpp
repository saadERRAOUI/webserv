//  Todo : Add includes from "./Includes/*" folder

/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/

#include "./Includes/parser.hpp"
#include <iostream>
#include "./Includes/Webserv.hpp"


void Setupserver(const WebServ &web)
{
    // std::cout << "Hello world.\n";
    
}

int main()
{
    try
    {
        WebServ web("./config.toml");
        web.getServers()[0].printServer();
        Setupserver(web);

    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        throw e;
    }
}
