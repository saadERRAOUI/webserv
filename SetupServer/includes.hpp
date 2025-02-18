#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
// #include <sys/epoll.h>
#include <sys/epoll.h>

#include <string.h>
#include <iostream>
// void Multiple_connections();
void Sockets_manager(int sockfd, int epollfd);
int create_manager();