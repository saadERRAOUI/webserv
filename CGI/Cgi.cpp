#include "Cgi.hpp"
#include "../HttpRequest/HttpRequest.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include "../Connection/Connection.hpp"
Cgi::Cgi(Route &route, HttpRequest &req, Connection *connection) : extension_table(route.getCGI()), route(route), request(req), childPid(0), outputProgress(0), inputProgress(0),
 state(IDLE), headersParsed(false), connection(*connection)
{

    this->input = "";
    this->output = "";
    this->binaryPath = "";
    this->env_set_up();
    std::cout << "Cgi constructor called\n";
    std::cout << "request path: " << request.getRequestURI() << "\n";
    std::cout << "request method: " << request.getMethod() << "\n";
    std::cout << "request body: " << request.getBody() << "\n";
    std::cout << "request content length: " << request.getHeader("Content-Length") << "\n"; 

    size_t pos = request.getRequestURI().find_last_of(".");
    if (pos == std::string::npos)
        throw CGIException("No extension was found in path!");
    std::cout << request.getRequestURI().substr(pos + 1) << "\n";
    std::map<std::string, std::string>::iterator val = this->extension_table.find(request.getRequestURI().substr(pos + 1));

    if (val == this->extension_table.end())
        throw CGIException("Extension Not Supported!");
    this->binaryPath = val->second;
    this->env_set_up();
    this->output = "/tmp/cgi" + generateRandomName();
    this->outputFileStream.open(this->output.c_str(), std::ios::out | std::ios::binary);
    if (this->request.getMethod() == "POST" && !this->request.getBody().empty())
    {
        this->input = "/tmp/cgi" + generateRandomName();
        this->inputFileStream.open(this->input.c_str(), std::ios::out | std::ios::binary);
        if (!inputFileStream.is_open())
            throw CGIException("Error: CGI: Input file stream failed to open");
    }
    std::cout << "binaryPath: " << binaryPath << "\n";
    std::cout << "script path: " << route.getCgiPath(this->request.getRequestURI()) << "\n";
    std::cout << "input: " << this->input << "\n";
    std::cout << "output: " << this->output << "\n";
    std::cout << "envp: \n";
    this->state  = this->request.getMethod() == "POST" && !this->request.getBody().empty() ? WRITING : FORKING;
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

std::string Cgi::generateRandomName() const
{
    std::string name = "tmp_";
    time_t t;
    srand((unsigned)time(&t));
    for (int i = 0; i < 10; i++)
        name += 'a' + rand() % 26;
    return name;
}

Cgi::Cgi(Cgi &other) : extension_table(other.extension_table), route(other.route), request(other.request), childPid(other.childPid)
, connection(other.connection){
    this->output = other.output;
    this->input = other.input;
    this->binaryPath = other.binaryPath;
}

void Cgi::execute()
{
    const size_t CHUNK_SIZE = 8000;

    if (state == WRITING) {
        const std::string &body = request.getBody();
        size_t remaining = body.size() - inputProgress;
        size_t toWrite = std::min(CHUNK_SIZE, remaining);

        if (toWrite > 0) {
            inputFileStream.write(body.data() + inputProgress, toWrite);
            inputProgress += toWrite;
        }

        if (inputProgress < body.size()) {
            return;
        }

        inputFileStream.close();
        state = FORKING;
    }

    if (state == FORKING) {
        if ((childPid = fork()) == -1) {
            throw CGIException("Error: CGI: Fork failed");
        } else if (childPid == 0) {
            std::cout << "ok im forking";
            freopen(output.c_str(), "w+", stdout);
            if (!input.empty())
                freopen(input.c_str(), "r", stdin);

            std::string scriptPath = route.getCgiPath(request.getRequestURI());
            char *argv[] = {
                const_cast<char *>(binaryPath.c_str()),
                const_cast<char *>(scriptPath.c_str()),
                NULL
            };
            execve(binaryPath.c_str(), argv, envp.data());
            perror("execve failed");
            std::exit(1);
        }

        state = WAITING;
        return;
    }

    if (state == WAITING) {
        int status;
        pid_t result = waitpid(childPid, &status, WNOHANG);
        if (result == 0) {
            return;
        } else if (result == -1) {
            throw CGIException("Error: CGI: waitpid failed");
        } else {
            childPid = 0;
            state = READING;
        }
    }
    
    if (state == READING) {
        if (!processOutputChunk(connection.Getfd())) {
            throw CGIException("Error: CGI: Failed to read output");
        }
    }
    if (state == DONE)
        connection.SetBool(true);
}

void Cgi::env_set_up()
{
    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"] = request.getMethod();
    env["SCRIPT_NAME"] = route.getCgiPath(this->request.getRequestURI());
    env["PATH_INFO"] = request.getRequestURI();
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

    std::map<std::string, std::string> headers = request.getHeaders();
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

bool Cgi::processOutputChunk(int clientFd)
{
    char buffer[4096];

    if (!outputFileStream.is_open())
        outputFileStream.open(output.c_str(), std::ios::in | std::ios::binary);
    std::cout << output.c_str();
    if (!outputFileStream.is_open())
        return std::cout << "failed here", false;

    outputFileStream.seekg(outputProgress);
    outputFileStream.read(buffer, sizeof(buffer));
    std::streamsize bytesRead = outputFileStream.gcount();

    if (bytesRead <= 0) return std::cout << "failed here2", false;
    
    outputProgress += bytesRead;
    responseBuffer.append(buffer, bytesRead);
    if (!headersParsed) {
        int pos = responseBuffer.find("\r\n\r\n");
        std::cout << "\n" << pos << "<------\n";
        if (pos == -1)
            return true;

        std::string headersPart = responseBuffer.substr(0, pos + 4);
        parseHeaders(headersPart);

        headersParsed = true;

        std::string httpHeader = buildHttpResponseHeaders();
        std::cout << httpHeader << "\n";
        write(clientFd, httpHeader.c_str(), httpHeader.size());

        responseBuffer.erase(0, pos + 4);
    }

    if (!responseBuffer.empty()) {
        std::cout << "\n---->" << responseBuffer.c_str() << "<----\n";
        ssize_t written = write(clientFd, responseBuffer.c_str(), responseBuffer.size());
        std::cout << written << "<----\n";
        if (written > 0)
            responseBuffer.erase(0, written);
        state = DONE;
    }
    return true;
}

void Cgi::parseHeaders(const std::string& rawHeaders)
{
    std::istringstream stream(rawHeaders);
    std::string line;

    statusCode = 200;
    statusMessage = "OK";
    
    while (std::getline(stream, line) && line != "\r")
    {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.resize(line.size() - 1);
        }

        size_t colon = line.find(":");
        if (colon == std::string::npos)
            continue;

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        size_t first = value.find_first_not_of(" ");
        if (first != std::string::npos)
            value = value.substr(first);

        if (key == "Status")
        {
            size_t space = value.find(" ");
            if (space != std::string::npos)
            {
                statusCode = std::atoi(value.substr(0, space).c_str());
                statusMessage = value.substr(space + 1);
            }
            else
            {
                statusCode = std::atoi(value.c_str());
                statusMessage = "";
            }
        }
        else
        {
            cgiHeaders[key] = value;
        }
    }
}
std::string Cgi::buildHttpResponseHeaders() const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = cgiHeaders.begin(); it != cgiHeaders.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }

    response << "\r\n";

    return response.str();
}

Cgi::~Cgi()
{
    if (childPid > 0)
    {
        kill(childPid, SIGTERM);
        waitpid(childPid, NULL, WNOHANG );
    }
}
