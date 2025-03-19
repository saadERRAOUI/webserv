/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/

// #include "HttpRequest/HttpRequest.hpp"
#include "Webserv.hpp"
#include "server.hpp"
#include <string>
#include <sstream>

void SetUpServer(Server to_create, int port)
{
    int f, fd;
    struct addrinfo hints, *res, *iter;
    std::stringstream ss;
    const int enable = 1;

    ss << port;
    memset(&hints,  0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((f = getaddrinfo(to_create.getHost().c_str(), ss.str().c_str() , &hints, &res)) != 0)
        std::cerr << "Error getaddrinfo: " << gai_strerror(f) << '\n';
    for (iter = res; iter != NULL; iter = iter->ai_next){
        fd = socket(iter->ai_family, iter->ai_socktype , iter->ai_protocol);
            if (fd == -1)
                continue;
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
            {
                std::cerr << "Error setsocketopt: " << strerror(errno) << '\n';
                close(fd);
                continue;
            }
            if(bind(fd, iter->ai_addr, iter->ai_addrlen) < 0)
            {
                std::cerr << "ERROR BIND: " << strerror(errno) << "\n";
                close(fd);
                continue;
            }
            break ;
    }
    std::cout << "After loop fd: " << fd << "\n";
    freeaddrinfo(res);
    if (iter == NULL)
    {
        std::cerr << "cannot bind: " << strerror(errno) << "\n";
        return ;
    }
    if (listen(fd, 2) == -1)
        std::cerr << "listen: " << strerror(errno) << '\n';
}

void Socketcreate(WebServ *web)
{
    // std::vector<Server> tmp = web->getServers();
    // std::cout << "===============\n";
    for (std::vector<Server>::iterator it = web->getServers().begin(); it != web->getServers().end(); it++)
    {
        for (std::vector<int>::iterator P_it = it->getPorts().begin(); P_it != it->getPorts().end(); P_it++)
            SetUpServer(*it, *P_it);
    }
    // std::cout << "=====> " << web->getServers().size() << "   ==========\n";
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
