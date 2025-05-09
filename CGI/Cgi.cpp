#include "Cgi.hpp"

#include <unistd.h>
#include <sys/wait.h>
Cgi::Cgi(Route &route, Request &req) : extension_table(route.getCGI()), route(route), request(req), childPid(0)
{
    size_t pos = request.getPath().find_last_of(".");
    if (pos == std::string::npos)
        throw CGIException("No extension was found in path!");
    std::cout << request.getPath().substr(pos + 1) << "\n";
    std::map<std::string, std::string>::iterator val = this->extension_table.find(request.getPath().substr(pos + 1));

    if (val == this->extension_table.end())
        throw CGIException("Extension Not Supported!");
    this->binaryPath = val->second;
}

std::string generateRandomName()
{
    std::string name = "tmp_";
    time_t t;
    srand((unsigned)time(&t));
    for (int i = 0; i < 10; i++)
        name += 'a' + rand() % 26;
    return name;
}

Cgi::Cgi(Cgi &other) : extension_table(other.extension_table), route(other.route), request(other.request), childPid(other.childPid)
{
    this->output = other.output;
    this->input = other.input;
    this->binaryPath = other.binaryPath;
}

void Cgi::execute()
{
    this->output = "/tmp/cgi" + generateRandomName();
    int &pid = childPid;

    if (this->request.getMethod() == "POST" && !this->request.getBody().empty())
    {
        this->input = "/tmp/cgi" + generateRandomName();
        size_t contentLength = std::strtoul(this->request.getHeader("Content-Length").c_str(), NULL, 10);

        std::ofstream inputFileStream(this->input.c_str(), std::ios::out | std::ios::binary);
        if (inputFileStream.is_open())
        {
            inputFileStream.write(this->request.getBody().c_str(), contentLength);
            inputFileStream.close();
        }
    }
    if ((pid = fork()) == -1)
        throw CGIException("Error: CGI: Fork failed");
    else if (pid == 0)
    {
        std::cout << "here!";
        std::cout << "binaryPath: " << binaryPath << "\n";
        std::cout << "script path: " << route.getCgiPath(this->request.getPath()) << "\n";
        freopen(output.c_str(), "w+", stdout);
        if (!this->request.getBody().empty())
            if (!this->input.empty() && freopen(this->input.c_str(), "r", stdin) == NULL)
                std::exit(1);
        std::string scriptPath = route.getCgiPath(this->request.getPath());
        char *argv[] = {
            const_cast<char *>(binaryPath.c_str()),
            const_cast<char *>(scriptPath.c_str()),
            NULL};

        std::cout << "here!";
        execve(binaryPath.c_str(), argv, this->envp.data());
        perror("execve failed");
        std::exit(1);
    }
    else
        waitpid(pid, NULL,0);
}

void Cgi::env_set_up()
{
    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"] = request.getMethod();
    env["SCRIPT_NAME"] = route.getCgiPath(this->request.getPath());
    env["PATH_INFO"] = request.getPath();
    env["CONTENT_TYPE"] = request.getHeader("Content-Type");
    env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    env["SERVER_PROTOCOL"] = request.getVersion();
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["REDIRECT_STATUS"] = "200";

    std::ostringstream query;
    const std::map<std::string, std::string> &q = request.getQueryParams();
    for (std::map<std::string, std::string>::const_iterator it = q.begin(); it != q.end(); ++it)
    {
        if (it != q.begin())
            query << "&";
        query << it->first << "=" << it->second;
    }
    env["QUERY_STRING"] = query.str();

    std::map<std::string, std::string> &headers = request.getHeaders();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::string key = it->first;
        for (size_t i = 0; i < key.size(); ++i)
        {
            if (key[i] == '-')
                key[i] = '_';
            else
                key[i] = toupper(key[i]);
        }
        env["HTTP_" + key] = it->second;
    }
    envp.clear();
    for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it)
    {
        std::string entry = it->first + "=" + it->second;
        char *str = new char[entry.size() + 1];
        std::strcpy(str, entry.c_str());
        envp.push_back(str);
    }
    envp.push_back(NULL);
}

std::string Cgi::getResponse()
{
	std::string response;
	char buffer[4096];
	ssize_t bytesRead;

	std::ifstream inFile(output.c_str(), std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		(remove(output.c_str()) , remove(this->input.c_str()) , remove(this->output.c_str()));
		return response;
	}
	while ((bytesRead = inFile.readsome(buffer, sizeof(buffer))))
		response.append(buffer, bytesRead);
	inFile.close();
	(remove(output.c_str()) , remove(input.c_str()) , remove(output.c_str()));
	return response;
}



Cgi::~Cgi()
{
    if (childPid > 0)
    {
        kill(childPid, SIGTERM);
        waitpid(childPid, NULL, WNOHANG );
    }
}