#include "Cgi.hpp"



Cgi::Cgi(const Route &route, const Request& req) : extension_table(route.getCGI()), route(route), request(req)
{

}

void Cgi::env_set_up() {
    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"]     = request.getMethod();
    env["SCRIPT_NAME"]        = route.getCgiPath(this->request.getPath());
    env["PATH_INFO"]          = request.getPath();
    env["CONTENT_TYPE"]       = request.getHeader("Content-Type");
    env["CONTENT_LENGTH"]     = request.getHeader("Content-Length");
    env["SERVER_PROTOCOL"]    = request.getVersion();
    env["GATEWAY_INTERFACE"]  = "CGI/1.1";
    env["REDIRECT_STATUS"]    = "200";

    std::ostringstream query;
    const std::map<std::string, std::string>& q = request.getQueryParams();
    for (std::map<std::string, std::string>::const_iterator it = q.begin(); it != q.end(); ++it) {
        if (it != q.begin()) query << "&";
        query << it->first << "=" << it->second;
    }
    env["QUERY_STRING"] = query.str();

    std::map<std::string, std::string> headers = request.getHeaders();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string key = it->first;
        for (size_t i = 0; i < key.size(); ++i) {
            if (key[i] == '-') key[i] = '_';
            else key[i] = toupper(key[i]);
        }
        env["HTTP_" + key] = it->second;
    }
    envp.clear();
    for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
        std::string entry = it->first + "=" + it->second;
        char* str = new char[entry.size() + 1];
        std::strcpy(str, entry.c_str());
        envp.push_back(str);
    }
    envp.push_back(nullptr);
}



Cgi::~Cgi() {
    for (size_t i = 0; i < envp.size(); ++i) {
        delete[] envp[i];
    }
}


