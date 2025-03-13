#include "webserv.hpp"
#include "utilities/utilities.hpp"
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

void WebServ::parseServer(Section &section)
{
    Server server(*this);
    for (std::deque<key_pair>::iterator key_val = section.key_val.begin(); key_val != section.key_val.end(); key_val++)
    {
        server.set(key_val->first, key_val->second);
    }

}

WebServ::WebServ(std::string config_file) : parser(config_file)
{
    this->config_file = config_file;
    parser.parse();
    for (std::map<std::string, std::deque<Section> >::iterator sections = parser.globalSection.raw_data.begin(); sections != parser.globalSection.raw_data.end(); sections++)
    {
        if (sections->first != "server")
            throw std::invalid_argument("Invalid section name");
        for (std::deque<Section>::iterator section = sections->second.begin(); section != sections->second.end(); section++)
            this->parseServer(*section);
    }
}
