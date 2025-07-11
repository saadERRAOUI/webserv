/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sahazel <sahazel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:31 by serraoui          #+#    #+#             */
/*   Updated: 2025/07/11 16:24:21 by sahazel          ###   ########.fr       */
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
#include "HttpRequest/HttpRequestParser.hpp"
#include "SetupServer/includes.hpp"
#include <signal.h>

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
    request->setRequestURI(std::string("/api/v1/script.s"));
    request->setVersion(std::string("HTTP/1.1"));
    request->setHeaders(std::string("accept-encoding"), std::string("gzip, deflate, br"));
    request->setHeaders(std::string("Accept"), std::string("*/*"));
    request->setHeaders(std::string("User-Agent"), std::string("Thunder Client (https://www.thunderclient.com)"));
    request->setHeaders(std::string("Host"), std::string("example.com"));
    request->setHeaders(std::string("Connection"), std::string("close"));
    request->setIsCGI(true);
    // Host
    return (request);

}


void Print_static_Request(HttpRequest tmpReques){
    (void)tmpReques;
    // std::cout << "METHOD : " << tmpReques.getMethod() << '\n';
    // std::cout << "Request URI : " << tmpReques.getRequestURI() << '\n';
    // std::cout << "Version : " << tmpReques.getVersion() << '\n';
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
    HttpRequestParser parser;
    // todo remove this line
    std::ofstream file("received_data.txt", std::ios::out | std::ios::app);
    // todo remove this line

    struct epoll_event event;
    struct epoll_event events[MAX_EPOLL_EVENT];
    std::vector<int> sockservers;
    std::map<int , Connection> map_connections;
    char BUFFER[BUFFER_SIZE] = {0};

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
    // Timeout constant: 10 seconds in microseconds
    const long TIMEOUT_US = 10000000; // 10 seconds
    
    while (true)
    {
        // Calculate timeout for epoll_wait (1 second to allow periodic timeout checks)
        int epoll_timeout = 1000; // 1 second in milliseconds
        
        int n = epoll_wait(epollfd, events, MAX_EPOLL_EVENT, epoll_timeout);
        // std::cout << "number of event: " << n << '\n';
        if (n == -1)
        {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "epoll_wait Error: " << strerror(errno) << '\n';
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLIN) && is_server(events[i].data.fd, sockservers))
            {
                Connection tmp = Connection(events[i].data.fd, epollfd, web);
                map_connections[tmp.Getfd()] = tmp;
                // int to_check = tmp.Getfd();
                // std::cout << "fd client: " << to_check << '\n';
                // std::cout << "fd cliend: " << map_connections[to_check].Getfd() << '\n';
            }
            else if (map_connections.size() && map_connections.find(events[i].data.fd) != map_connections.end() && (events[i].events & EPOLLIN))
            {
                // Update timeout for this connection
                map_connections[events[i].data.fd].SetTimeout(get_current_time());
                
                int size = read(events[i].data.fd, BUFFER, BUFFER_SIZE);
                if (size < BUFFER_SIZE)
                {
                    map_connections[events[i].data.fd].ChagenMode(epollfd, events[i].data.fd, EPOLLOUT);
                    if (!file.is_open())
                    {
                        std::cerr << "Error opening file >>  " << strerror(errno) << '\n'; // todo build error
                        // return;
                    }
                    file.write(BUFFER, size);
                    file.close();
                }

                ParseResult result = parser.parse(map_connections[events[i].data.fd].GetRequest(), BUFFER, size);
                if (result == PARSE_ERROR) {
                    // Create a proper HttpResponse for error handling
                    HttpResponse *error_response = new HttpResponse(events[i].data.fd);
                    map_connections[events[i].data.fd].SetHttpResponse(error_response);
                    ErrorBuilder(&map_connections[events[i].data.fd], &map_connections[events[i].data.fd].Getserver(), 400);
                    delete error_response;
                    close(events[i].data.fd);
                    map_connections.erase(events[i].data.fd);
                    continue;
                }
                map_connections[events[i].data.fd].GetRequest().showRequest();
                ResponseBuilder(&map_connections[events[i].data.fd]);
            }
            else if (
                map_connections.size() &&
                map_connections.find(events[i].data.fd) != map_connections.end() &&
                (events[i].events & EPOLLOUT)
            )
                    ResponseBuilder(&map_connections[events[i].data.fd]);
            MonitorConnection(&map_connections, epollfd);
        }
        
        // Check for timed out connections after processing all events
        long current_time = get_current_time();
        std::map<int, Connection>::iterator conn_it = map_connections.begin();
        while (conn_it != map_connections.end()) {
            if (current_time - conn_it->second.GetTimeout() > TIMEOUT_US) {
                std::cout << "Connection " << conn_it->first << " timed out after 10 seconds" << std::endl;
                
                // Create HttpResponse for timeout
                HttpResponse *timeout_response = new HttpResponse(conn_it->first);
                conn_it->second.SetHttpResponse(timeout_response);
                
                // Use ErrorBuilder to send 408 response
                ErrorBuilder(&conn_it->second, &conn_it->second.Getserver(), 408);
                conn_it->second.SetBool(true);
                
                // Remove from epoll before closing
                struct epoll_event event;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_it->first, &event);
                
                delete timeout_response;
                close(conn_it->first);
                map_connections.erase(conn_it++);
            } else {
                ++conn_it;
            }
        }
    }
}

int main(int ac, char **av)
{
    try
    {
        if (ac != 2)
            throw std::invalid_argument("no argument provided");
        signal(SIGPIPE, SIG_IGN);
        WebServ web(av[1]);
        // web.getServers()[0].printServer();
        Socketcreate(&web);
        int epollfd = create_manager();
        manage_connections(&web, epollfd); //need web class .
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
