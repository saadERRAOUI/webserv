#include "HttpResponse.hpp"

/*
    Author: BOUZID Hicham
    Description: check if the URI containe the full LOCATION or not
    Date: 2025-04-25
*/

bool CompareRU(std::string URI, std::string location){
    std::string tmplocation = location;
    int index;

    if (tmplocation[location.length() - 1] != '/' && tmplocation != location)
        tmplocation += "/";
    for (index = 0; index < (int)URI.length() && index < (int)tmplocation.length(); index++){
        if (URI[index] != tmplocation[index])
            break;
    }
    if (index == (int)tmplocation.length())
        return (true);
    return (false);    
}

/*
    Author: BOUZID Hicham
    Description: match the request That we got with ours routes
    Date: 2025-04-24 
*/

std::string MatchRoutes(std::map<std::string, route> &TmpRoutes, HttpRequest &TmpRequest){
    std::string URI = TmpRequest.getRequestURI();
    std::map<std::string, route>::iterator it;

    for (it = TmpRoutes.begin();it != TmpRoutes.end(); it++){
        if (CompareRU(TmpRequest.getRequestURI(), it->first) == true)
            return (it->first);
    }

    return(std::string("ROUTE NOT FOUND TO SERVE !!!"));
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