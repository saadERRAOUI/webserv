#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include <algorithm>

#define MAX_EPOLL_EVENT 10
class Server{
    int fdsock;
    int port;
    std::string host;
    int flag;
    public:
        Server(int port, int flag);
        // Server(int fdsock);
        ~Server();

       int  Getsockfd() const;
       int  Getflag() const;

};
// void Multiple_connections();
void Sockets_manager(std::map<int , Server *> * sockets, int epollfd);
int create_manager();
int SetUpServer(int port);



// std::map<int , Server *> servers;