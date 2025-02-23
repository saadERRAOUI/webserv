/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:31 by serraoui          #+#    #+#             */
/*   Updated: 2025/02/23 17:45:32 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//  Todo : Add includes from "./Includes/*" folder

/* 
    Author      : Saad ERRAOUI
    Date        : 2024-12-22
    Description : main method 
*/
#include <iostream>
#include <fstream>
#include "HttpRequest/HttpRequest.hpp"

int main() {
    HttpRequest httpRequest;
    std::ifstream requestFile("HttpRequest/request", std::ios::binary);

    if (!requestFile.is_open()) {
        std::cerr << "Failed to open request.txt" << std::endl;
        return 1;
    }

    char byte;
    while (requestFile.get(byte)) {
        httpRequest.parseHttpRequestOrchestrator(byte);
    }

    std::map<std::string, std::string> headers = httpRequest.getHeaders();
    std::cout << "Parsed HTTP Headers: " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    
    
    std::cout << "Parsed HTTP Body: " << httpRequest.getState() << std::endl;
    std::cout << httpRequest.getBody() << std::endl;
    requestFile.close();
    return 0;
}