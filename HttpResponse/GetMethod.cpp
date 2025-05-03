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
    if (URI[URI.length() - 1] != '/')
        URI += "/";
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
    Description: check if  method of request allowed or not
    Date: 2025-04-25
*/
bool MethodAllowed(std::vector<std::string> list, std::string method)
{
    std::vector<std::string>::iterator it = find(list.begin(), list.end(), method);
    if (it != list.end())
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
        if (CompareRU(TmpRequest.getRequestURI(), it->first) == true){
            if(MethodAllowed(TmpRoutes[it->first].getMethods(), "GET") == false)
                return (std::string("405"));
            else
                return (it->first);
        }
    }
    return(std::string("404"));
}

/*
    Author: BOUZID Hicham
    Description: created to serve default index.html
                if we have right else return error
    Date: 2025-04-29
*/
std::string ServerNormal(Connection *Infos, std::string URI, std::string route, int code)
{
    std::string ActualPath;

    ActualPath = std::string(".") + Infos->Getserver().getRoutes()[route].getRoot() + URI;
    std::cout << "From this path we will serve: " << ActualPath << '\n';
    if (access(ActualPath.c_str(), R_OK))
        return(ErrorBuilder(Infos, &Infos->Getserver(), (std::string("Permission denied") == std::string(strerror(errno)) ? 403: 404)));
    std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();
	std::string response = Infos->GetRequest().getVersion();
    if (code != 200)
	    ActualPath = chose_one(Infos->Getserver().webServ.getErrorPages()[code], Infos->Getserver().getErrorPages()[code]);
	response += " " + tostring(code) + " ";
	response += Infos->GetResponse().GetStatusCode(code);
	response += "\r\n";
	for (std::map<std::string, std::string>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
	{
		response += it->first;
		response += ": ";
		response += it->second;
		response += "\r\n";
	}
	std::string rt = OpenFile(ActualPath, code);
	response += "Content-Length: " + tostring((int)rt.size());
	response += "\r\n\r\n";
	response += rt;
	if (code != 301)
		Infos->SetBool(true);
	return (response);
}

/*
    Author: BOUZID Hicham
    Description: list file if autoindex on
                     and index file not found
    Date: 2025-05-01

*/

std::string ListFiles(Connection *Infos, std::string URI, std::string route, int code){
    std::string ActualPath;
    std::string RListing;
    DIR *dir;
    struct dirent *dp;

    // std::cout << "ENTER into list files: " << '\n';
    ActualPath = std::string(".") + Infos->Getserver().getRoutes()[route].getRoot() + URI;
    std::cout << "From this path we will serve: " << ActualPath << '\n';
    if ((dir = opendir(ActualPath.c_str())) == NULL)
        return (ErrorBuilder(Infos, &Infos->Getserver(), (std::string("Permission denied") == std::string(strerror(errno)) ? 403 : 404)));
    RListing =  "<!DOCTYPE html><html><head><title>Index of " + URI + "</title></head><body>\n";
    RListing += "<h1>Index of " + URI + "</h1><hr><pre>\n";
    dp = readdir(dir);
    while ((dp = readdir(dir)) != NULL)
    {

        RListing += "<a href=\"" + ((dp->d_type == 8) ? std::string(dp->d_name) : std::string(dp->d_name) + "/") + "\">" + ((dp->d_type == 8) ? std::string(dp->d_name) : std::string(dp->d_name) + "/") + "</a>\n";
    }
    RListing += "</pre><hr></body></html>\n";
    closedir(dir);
    std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();
    std::string response = Infos->GetRequest().getVersion();
    if (code != 200)
        ActualPath = chose_one(Infos->Getserver().webServ.getErrorPages()[code], Infos->Getserver().getErrorPages()[code]);
    response += " " + tostring(code) + " ";
    response += Infos->GetResponse().GetStatusCode(code);
    response += "\r\n";
    for (std::map<std::string, std::string>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
    {
        response += it->first;
        response += ": ";
        response += it->second;
        response += "\r\n";
    }
    response += "Content-Length: " + tostring((int)RListing.size());
    response += "\r\n\r\n";
    response += RListing;
    if (code != 301)
    Infos->SetBool(true);

    return (response);
}

/*
    Author: BOUZID Hicham
    Description: Implementing GET method that
                simulate other webserves like nginx or apache
    Date: 2025-04-24
*/

std::string GetMethod(Connection *Infos)
{
    std::map<std::string, route> routes = Infos->Getserver().getRoutes();
    std::string result;

    result = MatchRoutes(routes, Infos->GetRequest());
    // in ths condition i checked for error pages or somthing wrong
    if (!Infos->Getserver().getErrorPages()[atoi(result.c_str())].empty() || !Infos->Getserver().webServ.getErrorPages()[atoi(result.c_str())].empty()){
        return (ErrorBuilder(Infos, &Infos->Getserver(), atoi(result.c_str())));
    }
    // here we check for ace
    else
    {
      // Build  add to request URI slash + build response 301
      if (result  == (Infos->GetRequest().getRequestURI() + std::string("/"))){
          Infos->GetRequest().setHeaders(std::string("Location"),
              Infos->GetRequest().getRequestURI() + std::string("/"));
              return (ErrorBuilder(Infos, &Infos->Getserver(), 301));
      }
      else if (result == Infos->GetRequest().getRequestURI()){
          if (!Infos->Getserver().getRoutes()[result].getRedirection().empty())
          {
              if (Infos->SetRedirect(Infos->Getserver().getRoutes()[result].getRedirection()) == true){
                  std::cout <<  "this is the path of redirection: " << Infos->Getserver().getRoutes()[result].getRedirection() << '\n';
                  Infos->GetRequest().setHeaders(std::string("Location"),
                     Infos->Getserver().getRoutes()[result].getRedirection());
                      return (ErrorBuilder(Infos, &Infos->Getserver(), 301));
              }
              // response with error an set the connection done
              else
                  // std::cout << "Not Allowed .\n";
                      return (ErrorBuilder(Infos, &Infos->Getserver(), 404));
          }
          else if (Infos->Getserver().getRoutes()[result].getIndex().empty()){
              return (ServerNormal(Infos, Infos->GetRequest().getRequestURI() +
                  Infos->Getserver().getRoutes()[result].getIndex(), result, 200));
              // Ser
              // serve the index file.
              // normale if found
          }
          else{
              if (Infos->Getserver().getRoutes()[result].getAutoindex() == true){
                  std::cout << "++++++++++++++++++++++++++\n";
                  return (ListFiles(Infos, Infos->GetRequest().getRequestURI(), result, 200));
                  // list all files
              }
              else
                  return (ErrorBuilder(Infos, &Infos->Getserver(), 403));
          }
      }
        // serve file if have a right permition
        else{

        }

    }
    return (std::string("empty"));
    //Check for all routes and autoindex
}
