/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:31 by serraoui          #+#    #+#             */
/*   Updated: 2025/03/17 00:39:35 by hitchman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  Todo : Add includes from "./Includes/*" folder

/*
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method
*/
// #include <iostream>
// #include <fstream>
// #include "HttpRequest/HttpRequest.hpp"

// int main() {
//     HttpRequest httpRequest;
//     std::ifstream requestFile("HttpRequest/request", std::ios::binary);

//     if (!requestFile.is_open()) {
//         std::cerr << "Failed to open request.txt" << std::endl;
//         return 1;
//     }

//     char byte;
//     while (requestFile.get(byte)) {
//         httpRequest.parseHttpRequestOrchestrator(byte);
//     }

//     std::map<std::string, std::string> headers = httpRequest.getHeaders();
//     std::cout << "Parsed HTTP Headers: " << std::endl;
//     for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
//         std::cout << it->first << ": " << it->second << std::endl;
//     }


//     std::cout << "Parsed HTTP Body: " << httpRequest.getState() << std::endl;
//     std::cout << httpRequest.getBody() << std::endl;
//     requestFile.close();
//     return 0;
//  }
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
        it->setSocket(SetUpServer(it->getPorts()));
    //    int tmp =  SetUpServer((srd)it->getPorts());
        // std::vector<int>f = it->getPorts();
        // std::cout << "========== " << it->getPorts().size() << " ===========\n";
        // std::cout << "========== " << f.size() << " ===========\n";
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
