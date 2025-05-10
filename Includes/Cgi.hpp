#pragma once

#include <ctime>
#include <map>
#include <string>
#include "server.hpp"
#include <cstring>
#include <sstream>
#include "../HttpRequest/HttpRequest.hpp"

    class Cgi {
        private:
            std::time_t start;
            std::vector<char*> envp;
            std::map<std::string, std::string> extension_table;
            Route& route;
            HttpRequest& request;
            std::string output;
            std::string input;
            int childPid;
            
            std::string binaryPath;
        public:

            Cgi(Route &route, HttpRequest &req);
            Cgi(const Cgi& other);
            ~Cgi();
            Cgi& operator=(const Cgi& other);
            Cgi(Cgi &other);
            void execute();
            void env_set_up();
            std::string getResponse();
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
        