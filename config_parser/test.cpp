#include  "parser.hpp"
#include <iostream>
#include "./webserv.hpp"


int main()
{
    try {
        WebServ web("./config.toml");
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}