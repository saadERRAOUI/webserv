#include "Webserv.hpp"
#include "utilities.hpp"
#include <algorithm>

void Server::set(std::string &key, TOMLValue &val)
{
    if (key == "port" && val.type == TOMLValue::ARRAY)
        transform(val.array->begin(), val.array->end(), std::back_insert_iterator<std::vector<int> >(this->port), parseInt);
    else if (key == "host" && val.type == TOMLValue::SINGLE)
        this->setHost(*val.single);
    else if (key == "server_name" && val.type == TOMLValue::ARRAY)
        this->setServerName(std::vector<std::string>(val.array->begin(), val.array->end()));
    else if (key == "max_body_size" && val.type == TOMLValue::SINGLE)
        this->setMaxBodySize(parseInt(*val.single));
    else
        throw std::invalid_argument("Invalid key/value");
}

route WebServ::parseRoute(Section &section)
{
    route Route;
    for (std::deque<key_pair>::iterator key_val = section.key_val.begin(); key_val != section.key_val.end(); key_val++)
    {
        if (key_val->first == "path" && key_val->second.type == TOMLValue::SINGLE)
        Route.setPath(*key_val->second.single);
        else if (key_val->first == "index" && key_val->second.type == TOMLValue::SINGLE)
            Route.setIndex(*key_val->second.single);
        else if (key_val->first == "methods" && key_val->second.type == TOMLValue::ARRAY)
            Route.setMethods(std::vector<std::string>(key_val->second.array->begin(), key_val->second.array->end()));
        else if (key_val->first == "root" && key_val->second.type == TOMLValue::SINGLE)
            Route.setRoot(*key_val->second.single);
        else if (key_val->first == "redirection" && key_val->second.type == TOMLValue::SINGLE)
            Route.setRedirection(*key_val->second.single);
        else if (key_val->first == "autoindex" && key_val->second.type == TOMLValue::BOOL)
            Route.setAutoindex(key_val->second.true_false);
        else
            throw std::invalid_argument("Invalid key/value in route");
    }
    return Route;
}

Server WebServ::parseServer(Section &section)
{
    Server server(*this);
    for (std::deque<key_pair>::iterator key_val = section.key_val.begin(); key_val != section.key_val.end(); key_val++)
        server.set(key_val->first, key_val->second);
    for (std::map<std::string, std::deque<Section> >::iterator sections = section.raw_data.begin(); sections != section.raw_data.end(); sections++)
    {
        if (sections->first != "route" && sections->first != "error_pages")
            throw std::invalid_argument("Invalid section name");
        for (std::deque<Section>::iterator section = sections->second.begin(); section != sections->second.end(); section++)
        {
            if (section->name == "route")
            {
                route Route = parseRoute(*section);
                if (Route.getPath().empty())
                    throw std::invalid_argument("Invalid route path");
                server.getRoutes()[Route.getPath()] = Route;
            }
            else if (section->name == "error_pages")
            {
                int error_code;
                std::string path;
                for (std::deque<key_pair>::iterator key_val = section->key_val.begin(); key_val != section->key_val.end(); key_val++)
                {
                    if (key_val->second.type == TOMLValue::SINGLE)
                        error_code = parseInt(key_val->first);
                    else
                        throw std::invalid_argument("Invalid key/value in error_page");
                    server.getErrorPages()[error_code] = *key_val->second.single;

                }
            }
        }
    }
    // for (std::map<std::string, route>::iterator it = server.getRoutes().begin(); it != server.getRoutes().end(); it++)
    // {
    //     std::cout << it->first << std::endl;
    //     std::cout << it->second.getPath() << std::endl;
    //     std::cout << it->second.getIndex() << std::endl;
    //     for (std::vector<std::string>::iterator it2 = it->second.getMethods().begin(); it2 != it->second.getMethods().end(); it2++)
    //         std::cout << *it2 << std::endl;
    //     std::cout << it->second.getRoot() << std::endl;
    //     std::cout << it->second.getRedirection() << std::endl;
    //     std::cout << it->second.getAutoindex() << std::endl;
    // }

    return server;
}

WebServ::WebServ(std::string config_file) : parser(config_file)
{
    this->config_file = config_file;
    parser.parse();
    for (std::deque<key_pair>::iterator key_val = parser.globalSection.key_val.begin(); key_val != parser.globalSection.key_val.end(); key_val++)
    {
        if (key_val->first == "default_max_body_size" && key_val->second.type == TOMLValue::SINGLE)
            this->default_max_body_size = parseInt(*key_val->second.single);
        else
            throw std::invalid_argument("Invalid key/value");
    }
    for (std::map<std::string, std::deque<Section> >::iterator sections = parser.globalSection.raw_data.begin(); sections != parser.globalSection.raw_data.end(); sections++)
    {
        if (sections->first != "server")
            throw std::invalid_argument("Invalid section name");
        for (std::deque<Section>::iterator section = sections->second.begin(); section != sections->second.end(); section++)
            this->servers.push_back(parseServer(*section));
    }
}

std::vector<Server> WebServ::getServers()
{
    return this->servers;
}