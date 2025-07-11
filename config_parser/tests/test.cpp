#include  "parser.hpp"
#include <iostream>
#include "Webserv.hpp"
#include <gtest/gtest.h>



TEST(parse, ParsesCorrectInput) {
    WebServ webServ("config.toml");
    std::vector<Server> &servers = webServ.getServers();
    std::vector<int> f = servers[0].getPorts();
    ASSERT_EQ(servers.size(), 1);
    ASSERT_EQ(servers[0].getPorts().size(), 2);
    EXPECT_EQ(servers[0].getHost(), "123.0.0.0");
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


TEST(syntax_error__unterminatedString_Test , unterminatedString) {
    ASSERT_THROW(WebServ webServ("./syntax_error/unterminatedString.toml"), ConfigParser::SyntaxError);
}

TEST(syntax_error_incorrect_array , incorrect_array) {
    ASSERT_THROW(WebServ webServ("./syntax_error/incorrect_array.toml"), ConfigParser::SyntaxError);
}


TEST(syntax_error_incorrect_header , incorrect_header) {
    ASSERT_THROW(WebServ webServ("./syntax_error/incorrect_header.toml"), ConfigParser::SyntaxError);
}


TEST(INVALID_SERVER_NAME , InvalidServerName) {
    ASSERT_THROW(WebServ webServ("./invalid_names/invalid_servername.toml"), std::invalid_argument);
}

TEST(INVALID_KEY , InvalidKEYPAIR) {
    ASSERT_THROW(WebServ webServ("./invalid_names/invalid_keypair.toml"), std::invalid_argument);
}


TEST(INVALID_DATA_TYPES, InvalidPortType) {
    ASSERT_THROW(WebServ webServ("./invalid_data_types/invalid_port_type.toml"), std::invalid_argument);
}

TEST(INVALID_DATA_TYPES, InvalidMaxBodySizeType) {
    ASSERT_THROW(WebServ webServ("./invalid_data_types/invalid_max_body_size_type.toml"), std::invalid_argument);
}