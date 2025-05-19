/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:31 by serraoui          #+#    #+#             */
/*   Updated: 2025/05/08 15:36:38 by hitchman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  Todo : Add includes from "./Includes/*" folder

/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/


#include "Webserv.hpp"
#include "server.hpp"
#include "Connection/Connection.hpp"
#include "HttpRequest/HttpRequest.hpp"
#include "HttpResponse/HttpResponse.hpp"
#include "SetupServer/includes.hpp"


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
    HttpRequest  *request  = new HttpRequest;
    // std::string method = "GET"
    request->setMethod(std::string("GET"));
    request->setRequestURI(std::string("/api/v1/video.mp4"));
    request->setVersion(std::string("HTTP/1.1"));
    request->setHeaders(std::string("accept-encoding"), std::string("gzip, deflate, br"));
    request->setHeaders(std::string("Accept"), std::string("*/*"));
    request->setHeaders(std::string("User-Agent"), std::string("Thunder Client (https://www.thunderclient.com)"));
    request->setHeaders(std::string("Host"), std::string("example.com"));
    request->setHeaders(std::string("Connection"), std::string("close"));
    // Host
    return (request);

}


void Print_static_Request(HttpRequest tmpReques){
    std::cout << "METHOD : " << tmpReques.getMethod() << '\n';
    std::cout << "Request URI : " << tmpReques.getRequestURI() << '\n';
    std::cout << "Version : " << tmpReques.getVersion() << '\n';
    // std::cout << "=================== PRINT HEADERS ===================\n";
    // std::map<std::string, std::string> tmp_map = tmpReques.getHeaders();
    // for (std::map<std::string, std::string>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
    //     std::cout << it->first << ": " << it->second << '\n';
    // close(events[i].data.fd);
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
    char BUFFER[8000] = {0};

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
        std::cout << "number of event: " << n << '\n';
        if (n == -1)
        {
            std::cerr << "epoll_wait Error: " << strerror(errno) << '\n';
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLIN) && is_server(events[i].data.fd, sockservers))
            {
                Connection tmp = Connection(events[i].data.fd, epollfd, web);
                map_connections[tmp.Getfd()] = tmp;
                int to_check = tmp.Getfd();
                std::cout << "fd client: " << to_check << '\n';
                std::cout << "fd cliend: " << map_connections[to_check].Getfd() << '\n';
            }
            else if (map_connections.size() && map_connections.find(events[i].data.fd) != map_connections.end() && (events[i].events & EPOLLIN))
            {
                int size = read(events[i].data.fd, BUFFER, 8000);
                if (size < 8000)
                {
                    map_connections[events[i].data.fd].ChagenMode(epollfd, events[i].data.fd, EPOLLOUT);
                }
                HttpRequest *tmpRequest = ft_static_request();
                map_connections[events[i].data.fd].SetHttpRequest(tmpRequest);
                HttpResponse tmpHttpResponse(events[i].data.fd);
                map_connections[events[i].data.fd].SetHttpRespons(&tmpHttpResponse);
                ResponseBuilder(&map_connections[events[i].data.fd]);
            }
            else if (map_connections.size() && map_connections.find(events[i].data.fd) != map_connections.end() && (events[i].events & EPOLLOUT))
                    ResponseBuilder(&map_connections[events[i].data.fd]);
            MonitorConnection(&map_connections, epollfd);
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
