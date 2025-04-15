/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:31 by serraoui          #+#    #+#             */
/*   Updated: 2025/04/15 17:00:04 by hitchman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  Todo : Add includes from "./Includes/*" folder

/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/

// #include "HttpRequest/HttpRequest.hpp"
#include "Webserv.hpp"
#include "server.hpp"
#include "./Connection/Connection.hpp"
#include <string>
#include <sstream>

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
    std::cout << "epoll inctence was created .\n";
    return (EpollFd);
}

/*
    Author: BOUZID Hicham
    Description: check if the event come from a sever or client
    Date: 2025-04-07
*/

int is_server(int fdserver, std::vector<int> servers)
{
    std::vector<int>::iterator it;
    it = std::find(servers.begin(), servers.end(), fdserver);
    if (it != servers.end())
        return (1);
    return (0);
}


HttpRequest *ft_static_request(){
    HttpRequest *request = new HttpRequest;
    request->setMethod("GET");
    request->setRequestURI("/index.html");
    request->setVersion("HTTP/1.1");
    // request->_headers["Host"] = "example.com";
    // request->_headers["User-Agent"] = "curl/7.68.0";
    // request->_headers["Accept"] = "*/*";
    return (request);
    
}

/*
    Author: BOUZID hicham
    Description: add all servers socket to epoll
                 to insure listening for incomming
                 connections
    Date: 2025-04-07
*/


void manage_connections(WebServ *web, int epollfd)
{
    struct epoll_event event;
    struct epoll_event events[MAX_EPOLL_EVENT];
    std::vector<int> sockservers;
    std::map<int , Connection> map_connections;
    char BUFFER[1024] = {0};
    for (std::vector<Server>::iterator it = web->getServers()->begin(); it != web->getServers()->end(); it++)
    {
        for (std::vector<int>::iterator it1 = it->getSocket().begin(); it1 != it->getSocket().end(); it1++)
        {
            event.events = EPOLLIN;
            event.data.fd = *it1;
            if(epoll_ctl(epollfd, EPOLL_CTL_ADD, *it1, &event))
                std::cerr << "epoll ctl error: " << strerror(errno) << '\n';
            sockservers.push_back(*it1);
        }
    }
    while (true)
    {
        int n = epoll_wait(epollfd, events, MAX_EPOLL_EVENT, -1);
        if (n == -1)
        {
            std::cerr << "epoll_wait Error: " << strerror(errno) << '\n';
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLIN) && is_server(events[i].data.fd, sockservers))
            {
                Connection *tmp = new Connection(events[i].data.fd, epollfd, web);
                map_connections[tmp->Getfd()] = *tmp;
                delete tmp;
                int to_check = tmp->Getfd();
                std::cout << "fd client: " << to_check << '\n';
                std::cout << "fd cliend: " << map_connections[to_check].Getfd() << '\n';
            }
            else
            {
                read(events[i].data.fd, BUFFER, 1024);
                std::cout << "===========\n" << std::string(BUFFER) << '\n';
                // // connection object
                ft_static_request();
                // if (offset < 1024)
                // {
                //     map_connections[events[i].data.fd].AddRequest(std::string(BUFFER), true);
                //     // call for parsing request and then call the response
                //     // close(events[i].data.fd);
                //     // after reading all and building response object,
                //     // close fd and remove them from map of connections
                //     // erase connection map with fd client
                // }
                // else{
                //     // Arequest with false flag .
                //     map_connections[events[i].data.fd].AddRequest(std::string(BUFFER), false);
                //     continue;
                // }

            }
        }
    }
}

int main()
{
    try
    {
        WebServ web("./config.toml");
        // web.getServers()[0].printServer();
        Socketcreate(&web);
        int epollfd = create_manager();
        manage_connections(&web, epollfd); //need web class .
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        throw e;
    }

}
