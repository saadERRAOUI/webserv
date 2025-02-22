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
#define MAX_EPOLL_EVENT 10
// void Multiple_connections();
void Sockets_manager(int sockfd, int epollfd);
int create_manager();