#include "HttpResponse.hpp"
#include <sstream>
#include <sys/stat.h>

// Forward declarations
std::string RemovePrefix(std::string URI, std::string location, std::string root);

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
    
    std::cout << "[DEBUG] CompareRU: URI='" << URI << "' vs location='" << tmplocation << "'" << std::endl;
    
    // Check if URI starts with the location path
    if (URI.length() < tmplocation.length()) {
        std::cout << "[DEBUG] CompareRU: URI too short, no match" << std::endl;
        return false;
    }
    
    // Compare the location length characters
    for (index = 0; index < (int)tmplocation.length(); index++){
        if (URI[index] != tmplocation[index]) {
            std::cout << "[DEBUG] CompareRU: mismatch at position " << index << std::endl;
            return false;
        }
    }
    
    std::cout << "[DEBUG] CompareRU: matched " << index << " characters, location length=" << tmplocation.length() << std::endl;
    
    // Only return true if we matched the entire location path
    return true;
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

std::string MatchRoutes(std::map<std::string, Route> &TmpRoutes, HttpRequest &TmpRequest){
    std::string URI = TmpRequest.getRequestURI();
    std::map<std::string, Route>::iterator it;

    std::cout << "[DEBUG] MatchRoutes: checking URI='" << URI << "'" << std::endl;

    for (it = TmpRoutes.begin();it != TmpRoutes.end(); it++){
        std::cout << "[DEBUG] MatchRoutes: comparing with route='" << it->first << "'" << std::endl;
        if (CompareRU(TmpRequest.getRequestURI(), it->first) == true){
            std::cout << "[DEBUG] MatchRoutes: found match! checking methods..." << std::endl;
            if(MethodAllowed(TmpRoutes[it->first].getMethods(), "GET") == false)
                return (std::string("405"));
            else
                return (it->first);
        }
    }
    std::cout << "[DEBUG] MatchRoutes: no match found, returning 404" << std::endl;
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

    ActualPath = Infos->Getserver().getRoutes()[route].getRoot() + URI;
    std::cout << "From this path we will serve: " << ActualPath << '\n';
    if (access(ActualPath.c_str(), R_OK))
        return(ErrorBuilder(Infos, &Infos->Getserver(), (std::string("Permission denied") == std::string(strerror(errno)) ? 403: 404)));
    std::string response = Infos->GetRequest().getVersion();
    if (code != 200)
        ActualPath = chose_one(Infos->Getserver().webServ.getErrorPages()[code], Infos->Getserver().getErrorPages()[code]);
    response += " " + tostring(code) + " ";
    response += Infos->GetResponse().GetStatusCode(code);
    response += "\r\n";
    std::string rt = OpenFile(ActualPath, true, Infos, response);
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + tostring((int)rt.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
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
    ActualPath = RemovePrefix(URI, route, Infos->Getserver().getRoutes()[route].getRoot());
    if ((dir = opendir(ActualPath.c_str())) == NULL)
        return (ErrorBuilder(Infos, &Infos->Getserver(), (std::string("Permission denied") == std::string(strerror(errno)) ? 403 : 404)));
    RListing =  "<!DOCTYPE html><html><head><title>Index of " + URI + "</title></head><body>\n";
    RListing += "<h1>Index of " + URI + "</h1><hr><pre>\n";
    while ((dp = readdir(dir)) != NULL)
    {

        RListing += "<a href=\"" + ((dp->d_type == 8) ? std::string(dp->d_name) : std::string(dp->d_name) + "/") + "\">" + ((dp->d_type == 8) ? std::string(dp->d_name) : std::string(dp->d_name) + "/") + "</a>\n";
    }
    RListing += "</pre><hr></body></html>\n";
    closedir(dir);
    std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();
    std::string response = Infos->GetRequest().getVersion();
    response += " 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + tostring((int)RListing.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
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
    std::map<std::string, Route> routes = Infos->Getserver().getRoutes();
    std::string result;

    result = MatchRoutes(routes, Infos->GetRequest());
    std::cout << "[DEBUG] GetMethod: MatchRoutes returned '" << result << "'" << std::endl;
    
    // in ths condition i checked for error pages or somthing wrong
    if (!Infos->Getserver().getErrorPages()[atoi(result.c_str())].empty() || !Infos->Getserver().webServ.getErrorPages()[atoi(result.c_str())].empty()){
        std::cout << "[DEBUG] GetMethod: Found error page for " << result << ", calling ErrorBuilder" << std::endl;
        return (ErrorBuilder(Infos, &Infos->Getserver(), atoi(result.c_str())));
    }
    // here we check for ace
    else
    {
      // Check if we need to redirect to add trailing slash
      std::string requestURI = Infos->GetRequest().getRequestURI();
      if (!requestURI.empty() && requestURI[requestURI.size()-1] != '/') {
          std::string withSlash = requestURI + "/";
          // Check if there's a route that matches with the trailing slash
          if (routes.find(withSlash) != routes.end()) {
              Infos->GetRequest().setHeaders("Location", withSlash);
              return (ErrorBuilder(Infos, &Infos->Getserver(), 301));
          }
      }
      // Directory/index.html/autoindex logic for exact match
      if (requestURI == result) {
          if (!Infos->Getserver().getRoutes()[result].getRedirection().empty())
          {
              if (Infos->SetRedirect(Infos->Getserver().getRoutes()[result].getRedirection()) == true){
                  Infos->GetRequest().setHeaders(std::string("Location"),
                     Infos->Getserver().getRoutes()[result].getRedirection());
                      return (ErrorBuilder(Infos, &Infos->Getserver(), 301));
              }
              else
                      return (ErrorBuilder(Infos, &Infos->Getserver(), 404));
          }
          else{
              if (Infos->Getserver().getRoutes()[result].getAutoindex() == true)
              {
                  if (!Infos->Getserver().getRoutes()[result].getIndex().empty())
                  {
                      std::string root = Infos->Getserver().getRoutes()[result].getRoot();
                      std::string index = Infos->Getserver().getRoutes()[result].getIndex();
                      std::string indexPath = root;
                      if (!root.empty() && root[root.size()-1] != '/')
                          indexPath += "/";
                      indexPath += index;
                      if (access(indexPath.c_str(), R_OK) == 0)
                      {
                          return (ft_Get(Infos, "/" + index, result, 200));
                      }
                  }
                  return (ListFiles(Infos, Infos->GetRequest().getRequestURI(), result, 200));
              }
              else
              {
                  if (!Infos->Getserver().getRoutes()[result].getIndex().empty())
                  {
                      std::string indexPath = Infos->Getserver().getRoutes()[result].getRoot() + Infos->GetRequest().getRequestURI() + Infos->Getserver().getRoutes()[result].getIndex();
                      if (access(indexPath.c_str(), R_OK) == 0)
                      {
                          return (ft_Get(Infos, Infos->GetRequest().getRequestURI() + Infos->Getserver().getRoutes()[result].getIndex(), result, 200));
                      }
                  }
                  return (ErrorBuilder(Infos, &Infos->Getserver(), 403));
              }
          }
      }
      // Handle requests for files/directories under the route
      else if (requestURI.find(result) == 0) {
          // Check if the requested path is a directory
          std::string actualPath = RemovePrefix(requestURI, result, Infos->Getserver().getRoutes()[result].getRoot());
          struct stat st;
          std::cout << "[DEBUG] Checking if directory: " << actualPath << std::endl;
          if (stat(actualPath.c_str(), &st) == 0) {
              std::cout << "[DEBUG] stat() success, mode: " << st.st_mode << std::endl;
              if (S_ISDIR(st.st_mode)) std::cout << "[DEBUG] It is a directory!" << std::endl;
              else std::cout << "[DEBUG] Not a directory." << std::endl;
          } else {
              std::cout << "[DEBUG] stat() failed: " << strerror(errno) << std::endl;
          }
          if (stat(actualPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
              // It's a directory, apply autoindex/index logic
              if (Infos->Getserver().getRoutes()[result].getAutoindex() == true)
              {
                  if (!Infos->Getserver().getRoutes()[result].getIndex().empty())
                  {
                      std::string indexPath = actualPath;
                      if (!indexPath.empty() && indexPath[indexPath.size()-1] != '/')
                          indexPath += "/";
                      indexPath += Infos->Getserver().getRoutes()[result].getIndex();
                      if (access(indexPath.c_str(), R_OK) == 0)
                      {
                          return (ft_Get(Infos, requestURI + "/" + Infos->Getserver().getRoutes()[result].getIndex(), result, 200));
                      }
                  }
                  return (ListFiles(Infos, requestURI, result, 200));
              }
              else
              {
                  if (!Infos->Getserver().getRoutes()[result].getIndex().empty())
                  {
                      std::string indexPath = actualPath;
                      if (!indexPath.empty() && indexPath[indexPath.size()-1] != '/')
                          indexPath += "/";
                      indexPath += Infos->Getserver().getRoutes()[result].getIndex();
                      if (access(indexPath.c_str(), R_OK) == 0)
                      {
                          return (ft_Get(Infos, requestURI + "/" + Infos->Getserver().getRoutes()[result].getIndex(), result, 200));
                      }
                  }
                  return (ErrorBuilder(Infos, &Infos->Getserver(), 403));
              }
          } else {
              // It's a file, serve it normally
              return (ft_Get(Infos, requestURI, result, 200));
          }
      }
      return std::string("");
    }
    std::cout << "the result is: " << result << '\n';
    return (std::string(""));
    //Check for all routes and autoindex
}


/*
    Author: BOUZID Hicham
    Description: this function will remou
*/

std::string RemovePrefix(std::string URI, std::string location, std::string root){
    std::string Result;
    
    // If URI starts with "/" and doesn't contain the location path, it's likely an index file
    if (URI[0] == '/' && URI.find(location) == std::string::npos) {
        // This is an index file, just append to root
        Result = root + URI;
    } else {
        // Normal case: remove the location prefix from URI
        // Handle both "/root/" and "/root" cases
        std::string locationWithoutSlash = location;
        if (!locationWithoutSlash.empty() && locationWithoutSlash[locationWithoutSlash.size()-1] == '/') {
            locationWithoutSlash = locationWithoutSlash.substr(0, locationWithoutSlash.size()-1);
        }
        
        if (URI.find(locationWithoutSlash) == 0) {
            // URI starts with location (without trailing slash)
            Result = root + URI.substr(locationWithoutSlash.size());
        } else {
            // Fallback to original logic
            Result = root + URI.substr(location.size() - 1, URI.size());
        }
    }
    
    std::cout << "This is suffex of file: " << Result << "\n" ;
    return (Result);
}

/*
    Author: BOUZID Hicham
    Description: function return the content type of file
    Date: 2025-05-26
*/

std::string ContentType(std::string file){
    int index = file.size();
    std::string result;
    std::map<std::string, std::string> mimeTypes;

    // Populate the map
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".tar"] = "application/x-tar";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".wav"] = "audio/wav";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".avi"] = "video/x-msvideo";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".csv"] = "text/csv";
    mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

    // add map of extension and value content type
    for (int i = index - 1; i != 0; i--){
        if (file[i] == '.'){
            result = file.substr(i, index);
            break ;
        }
    }
    return (mimeTypes[result].size() ? mimeTypes[result] : std::string("text/html"));
}

/*
    Author: BOUZID Hicham
    Description: this function serve both small and hug file\
    Date: 2025-05-05
*/

std::string ft_Get(Connection *Infos, std::string URI, std::string route, int code){
    std::string ActualPath;
    std::string response;
    std::cout << "the request URI is: " << Infos->GetRequest().getRequestURI() << '\n';
    response = std::string("");
    if (!Infos->GetRequest().getVersion().empty())
    {
        // If URI ends with '/', append the index file
        std::string root = Infos->Getserver().getRoutes()[route].getRoot();
        std::string index = Infos->Getserver().getRoutes()[route].getIndex();
        if (!URI.empty() && URI[URI.size()-1] == '/' && !index.empty()) {
            if (!root.empty() && root[root.size()-1] != '/')
                ActualPath = root + "/" + index;
            else
                ActualPath = root + index;
        } else {
            ActualPath = RemovePrefix(URI, route, Infos->Getserver().getRoutes()[route].getRoot());
        }

        std::cout << "From this path we will serve : " << ActualPath << '\n';
        std::cout << "============> " << ActualPath.c_str() << "\n";
        if (access(ActualPath.c_str(), R_OK))
        {
            ErrorBuilder(Infos, &Infos->Getserver(), (std::string("Permission denied") == std::string(strerror(errno)) ? 403: 404));
            return("");
        }
        std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();
        response = Infos->GetRequest().getVersion();
        if (code != 200)
            ActualPath = chose_one(Infos->Getserver().webServ.getErrorPages()[code], Infos->Getserver().getErrorPages()[code]);
        response += " " + tostring(code) + " ";
        response += Infos->GetResponse().GetStatusCode(code);
        response += "\r\n";
        Infos->SetSize(GetLenght(ActualPath));
        response += "Content-Type: " +  ContentType(ActualPath) + "\r\n";
        response += "Content-Length: " + tostring(Infos->GetSize());
        response += "\r\n\r\n";
        
        // Read the file content and append to response
        OpenFile(ActualPath, true, Infos, response); 
        Infos->GetRequest().ClearURI();
        Infos->SetBool(true);  // Mark connection as done to prevent multiple serving
        return (response);
    }
    OpenFile(ActualPath, false, Infos, response);
    Infos->SetBool(true);  // Mark connection as done to prevent multiple serving
    return (std::string(""));
}


