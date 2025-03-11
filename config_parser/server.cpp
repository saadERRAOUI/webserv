
struct route {
    std::string path;
    std::string index;
    std::vector <std::string> methods;
    std::string root;
    std::string redirection;
    bool autoindex;
}
class Server 
{
    int port;
    //check if host is possible to be "localhost"
    std::string host;
    std::string server_name;
    std::map<int, std::string> error_pages;
    std::map<std::string, route> routes;
}





// std::map<std::string, route> 
