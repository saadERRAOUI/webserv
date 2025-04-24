#include "HttpResponse.hpp"

/*
    Author: BOUZID Hicham
    Description: match the request That we got with ours routes
    Date: 2025-04-24 
*/

std::string MatchRoutes(std::map<std::string, route> &TmpRoutes, HttpRequest &TmpRequest){
    std::string URI = TmpRequest.getRequestURI();
    std::map<std::string, route>::iterator it =  TmpRoutes.find(TmpRequest.getRequestURI());
    if (it != TmpRoutes.end())
        return (TmpRequest.getRequestURI());
    // it = TmpRoutes.begin();
    for (it = TmpRoutes.begin();it != TmpRoutes.end(); it++){
        // std::cout << "it: " << it->first << '\n';
    }
    // for (std::map<std::string, route>::iterator it = TmpRoutes.begin(); it != TmpRoutes.end(); it++){

    // }
    return(std::string(""));
}



/*
    Author: BOUZID Hicham
    Description: Implementing GET method that 
                simulate other webserves like nginx or apache
    Date: 2025-04-24
*/
std::string GetMethod(Connection *Infos){
    std::map<std::string, route> routes = Infos->Getserver().getRoutes();
    // Server *tmpServer = &Infos->Getserver();
    return (MatchRoutes(routes, Infos->GetRequest()));
    //Check for all routes and autoindex
}