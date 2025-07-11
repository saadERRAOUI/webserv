#include "includes.hpp"


int SetUpServer(Server to_create, int port)
{
    int f, fd;
    struct addrinfo hints, *res, *iter;
    std::stringstream ss;
    const int enable = 1;

    ss << port;
    memset(&hints,  0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((f = getaddrinfo(to_create.getHost().c_str(), ss.str().c_str(), &hints, &res)) != 0)
    {
        std::cerr << "Error getaddrinfo: " << gai_strerror(f) << '\n';
        exit(EXIT_FAILURE);
    }
    for (iter = res; iter != NULL; iter = iter->ai_next){
        fd = socket(iter->ai_family, iter->ai_socktype , iter->ai_protocol);
            if (fd == -1)
                continue;
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            {
                std::cerr << "Error setsocketopt: " << strerror(errno) << '\n';
                close(fd);
                continue;
            }
            if(bind(fd, iter->ai_addr, iter->ai_addrlen) < 0)
            {
                // std::cerr << "ERROR BIND: " << strerror(errno) << "\n";
                close(fd);
                continue;
            }
            break ;
    }
    freeaddrinfo(res);
    if (iter == NULL)
    {
        std::cerr << "cannot bind: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 2) == -1)
    {
        std::cerr << "listen: " << strerror(errno) << '\n';
        exit(1) ;
    }
    return (fd);
}

void Socketcreate(WebServ *web)
{
    for (std::vector<Server>::iterator it = web->getServers()->begin(); it != web->getServers()->end(); it++)
    {
        for (std::vector<int>::iterator P_it = it->getPorts().begin(); P_it != it->getPorts().end(); P_it++)
            it->setSocket(SetUpServer(*it, *P_it));
    }
}



/*
    Author: BOUZID hicham
    Description: craete epoll inctence to manange multiple sockets
    Date: 2025-02-18
*/

int create_manager()
{
    int EpollFd = epoll_create(10);
    if (EpollFd < 0)
    {
        std::cerr << "Epoll problem: " << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
    // std::cout << "epoll inctence was created .\n";
    return (EpollFd);
}
