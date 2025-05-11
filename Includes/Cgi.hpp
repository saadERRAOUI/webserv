#pragma once

#include <ctime>
#include <map>
#include <string>
#include "server.hpp"
#include <cstring>
#include <sstream>
#include "../HttpRequest/HttpRequest.hpp"
class Connection;
    class Cgi {
        private:
            typedef enum  {
                IDLE, WAITING, FORKING, READING, WRITING, DONE
            } e_state;
            std::time_t start;
            std::vector<char*> envp;
            std::map<std::string, std::string> extension_table;
            Route& route;
            HttpRequest& request;
            std::string output;
            int childPid;

            size_t outputProgress;

            size_t inputProgress;

            e_state state;
            std::ofstream inputFileStream;

            bool headersParsed;
            std::ifstream outputFileStream;
            std::string input;
            
            std::string responseBuffer;
            
            std::string binaryPath;
            int statusCode;
            Connection &connection;

std::string statusMessage;
std::map<std::string, std::string> cgiHeaders;
        public:

            Cgi(Route &route, HttpRequest &req, Connection *connection);
            Cgi(const Cgi& other);
            ~Cgi();
            Cgi& operator=(const Cgi& other);
            Cgi(Cgi &other);
            void execute();
            void env_set_up();
            bool processOutputChunk(int clientFd);
            std::string getResponse();
            void parseHeaders(const std::string& rawHeaders);
            std::string generateRandomName() const;
            std::string buildHttpResponseHeaders() const;
            class CGIException : public std::exception
            {
                std::string reason;
                public :
                CGIException(std::string reason) : reason(reason){};
                ~CGIException() throw() {};
                const char *what() const throw() 
                {
                    return this->reason.c_str();
                }
            };
        };
        