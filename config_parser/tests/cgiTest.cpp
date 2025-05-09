#include "server.hpp"

#include "Cgi.hpp"
#include "Webserv.hpp"

int main()
{
    WebServ webServ("config.toml");
    std::vector<Server> &servers = webServ.getServers();
    Request a;

    if (servers.size() >= 1)
        if (servers[0].getRoutes().size() >= 1)
        {
            Route &main = servers[0].getRoutes()["/api/v1"];
            Cgi cgi(main, a);
            cgi.env_set_up();
            cgi.execute();
            std::cout << cgi.getResponse();
            
        }
}