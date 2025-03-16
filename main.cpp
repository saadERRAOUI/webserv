//  Todo : Add includes from "./Includes/*" folder

/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/

#include "./Includes/parser.hpp"
#include "./Includes/Webserv.hpp"
#include "./Includes/server.hpp"
#include <iostream>
#include <vector>

int SetUpServer(std::vector<int> ports)
{
    std::cout << "--------------->\n";
    int Fd;
    struct sockaddr_in my_addr;
    Fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Fd < 0)
        std::cerr << "Socket Error: " << strerror(errno) << "\n";
    std::cout << "After creating a socket.\n";
    return (Fd);
}

void Socketcreate(WebServ *web)
{
    // std::vector<Server> tmp = web->getServers();
    for (std::vector<Server>::iterator it = web->getServers().begin(); it != web->getServers().end(); it++)
    {
    //    int tmp =  SetUpServer((srd)it->getPorts());
        std::vector<int>f = it->getPorts();
        std::cout  << "========== " << f.front() <<" ===========\n";
    }
}

int main()
{
    try
    {
        WebServ web("./config.toml");
        web.getServers()[0].printServer();
        Socketcreate(&web);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        throw e;
    }
}
