#include  "parser.hpp"
#include <iostream>
#include "Webserv.hpp"
#include <gtest/gtest.h>



TEST(parse, ParsesCorrectInput) {
    WebServ webServ("config.toml");
    std::vector<Server> servers = webServ.getServers();
    std::vector<int> f = servers[0].getPorts();
    for (std::vector<int>::iterator i= f.begin(); i != f.end(); i++)
        std::cout << *i << std::endl;
    ASSERT_EQ(servers.size(), 1);
    EXPECT_EQ(servers[0].getHost(), "localhost");
    ASSERT_EQ(servers[0].getPorts().size(), 2);
    EXPECT_EQ(servers[0].getPorts()[0], 12);
    EXPECT_EQ(servers[0].getPorts()[1], 123);
    ASSERT_EQ(servers[0].getServerName().size(), 1);
    EXPECT_EQ(servers[0].getServerName()[0], "example.com");
    EXPECT_EQ(servers[0].getMaxBodySize(), 1000000);
    ASSERT_EQ(servers[0].getRoutes().size(), 1);
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getPath(), "/api/v1");
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getIndex(), "index.html");
    ASSERT_EQ(servers[0].getRoutes().begin()->second.getMethods().size(), 1);
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getMethods()[0], "GET");
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getRoot(), "/var/www/html/pepe");
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getRedirection(), "");
    EXPECT_EQ(servers[0].getRoutes().begin()->second.getAutoindex(), true);
    ASSERT_EQ(servers[0].getErrorPages().size(),3);
    EXPECT_EQ(webServ.getDefaultMaxBodySize(), 123);
}  


TEST(INVALID_FILE, InvalidFile) {
    ASSERT_THROW(WebServ webServ("invalid.toml"), std::invalid_argument);
}


TEST(UnterminatedString, UnterminatedString) {
    ASSERT_THROW(WebServ webServ("unterminatedString.toml"), std::invalid_argument);
}

