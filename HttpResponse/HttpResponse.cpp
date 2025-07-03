#include "HttpResponse.hpp"
#include "../HttpRequest/HttpRequestParser.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>

std::string RemovePrefix(std::string URI, std::string location, std::string root);

// --- Helper Functions ---

static void sendErrorResponse(Connection *Infos, Server *TmpServer, int code) {
    std::string err = ErrorBuilder(Infos, TmpServer, code);
    write(Infos->Getfd(), err.c_str(), err.size());
    Infos->SetBool(true);
}

static void send405(Connection *Infos, const std::vector<std::string>& allowed_methods) {
    std::ostringstream oss;
    oss << "HTTP/1.1 405 Method Not Allowed\r\n";
    oss << "Allow: ";
    for (size_t i = 0; i < allowed_methods.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << allowed_methods[i];
    }
    oss << "\r\nContent-Type: text/plain\r\nContent-Length: 23\r\nConnection: close\r\n\r\nMethod Not Allowed\n";
    std::string response = oss.str();
    write(Infos->Getfd(), response.c_str(), response.size());
    Infos->SetBool(true);
}

static std::string getMimeExtension(const std::string& content_type) {
    static std::map<std::string, std::string> mimeTypes;
    if (mimeTypes.empty()) {
        mimeTypes["text/html"] = ".html";
        mimeTypes["text/css"] = ".css";
        mimeTypes["application/javascript"] = ".js";
        mimeTypes["application/json"] = ".json";
        mimeTypes["application/xml"] = ".xml";
        mimeTypes["image/jpeg"] = ".jpg";
        mimeTypes["image/png"] = ".png";
        mimeTypes["image/gif"] = ".gif";
        mimeTypes["image/svg+xml"] = ".svg";
        mimeTypes["application/pdf"] = ".pdf";
        mimeTypes["application/zip"] = ".zip";
        mimeTypes["application/x-tar"] = ".tar";
        mimeTypes["audio/mpeg"] = ".mp3";
        mimeTypes["audio/wav"] = ".wav";
        mimeTypes["video/mp4"] = ".mp4";
        mimeTypes["video/x-msvideo"] = ".avi";
        mimeTypes["text/plain"] = ".txt";
        mimeTypes["text/csv"] = ".csv";
        mimeTypes["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    }
    std::map<std::string, std::string>::iterator it = mimeTypes.find(content_type);
    return (it != mimeTypes.end()) ? it->second : ".bin";
}

static bool ensureDirectoryExists(const std::string& dir) {
    struct stat st;
    if (stat(dir.c_str(), &st) != 0) {
        if (mkdir(dir.c_str(), 0777) != 0) {
            std::cerr << "[DEBUG] Failed to create upload directory: " << dir << std::endl;
            return false;
        }
    }
    return true;
}

static bool saveUploadedFile(const std::string& upload_dir, const std::string& content_type, const std::string& body, std::string& out_path) {
    if (!ensureDirectoryExists(upload_dir)) return false;
    std::string ext = getMimeExtension(content_type);
    char filename[128];
    time_t now = time(0);
    sprintf(filename, "upload_%ld%s", (long)now, ext.c_str());
    out_path = upload_dir + filename;
    std::ofstream outfile(out_path.c_str(), std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(body.c_str(), body.size());
        outfile.close();
        return true;
    }
    std::cerr << "[DEBUG] Failed to open file for upload: " << out_path << std::endl;
    return false;
}

// --- Method Handlers ---

static void handleGet(Connection *Infos) {
    // Default GET handler
    std::string tmpstring = GetMethod(Infos);
    if (!tmpstring.empty())
        write(Infos->Getfd(), tmpstring.c_str(), tmpstring.size());
}

static void handlePost(Connection *Infos, Route& matchedRoute) {
    if (Infos->GetRequest().getIsChunked()) {
        std::string raw_body = Infos->GetRequest().getBody();
        if (raw_body.size() < 5 || raw_body.substr(raw_body.size() - 5) != "0\r\n\r\n") {
            return;
        }
        std::string decoded_body = decode_chunked_body(raw_body);
        Infos->GetRequest().setBody(decoded_body);
        std::string upload_dir = matchedRoute.getUpload();
        if (!upload_dir.empty()) {
            std::string content_type = Infos->GetRequest().getHeader("Content-Type");
            std::string full_path;
            if (saveUploadedFile(upload_dir, content_type, decoded_body, full_path)) {
                std::string response_body = "File uploaded to: " + full_path + "\n";
                char header[256];
                sprintf(header,
                    "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
                    (unsigned long)response_body.size());
                std::string response = std::string(header) + response_body;
                write(Infos->Getfd(), response.c_str(), response.size());
                Infos->SetBool(true);
                return;
            }
        }
        std::string response_body = "POST received!\n";
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/plain\r\n"
            << "Content-Length: " << response_body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << response_body;
        std::string response = oss.str();
        write(Infos->Getfd(), response.c_str(), response.size());
        Infos->SetBool(true);
        return;
    }
    std::map<std::string, std::string> headers = Infos->GetRequest().getHeaders();
    std::map<std::string, std::string>::iterator it = headers.find("content-length");
    if (it == headers.end()) {
        const char *length_required =
            "HTTP/1.1 411 Length Required\r\nContent-Type: text/html\r\nContent-Length: 53\r\n\r\n<html><body>411 Length Required</body></html>";
        write(Infos->Getfd(), length_required, strlen(length_required));
        Infos->SetBool(true);
        return;
    }
    int content_length = atoi(it->second.c_str());
    if (content_length <= 0) {
        const char *bad_request =
            "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 45\r\n\r\n<html><body>400 Bad Request</body></html>";
        write(Infos->Getfd(), bad_request, strlen(bad_request));
        Infos->SetBool(true);
        return;
    }
    std::string body = Infos->GetRequest().getBody();
    char buffer[8000];
    int total_read = body.size();
    while (total_read < content_length) {
        int to_read = content_length - total_read;
        if (to_read > 8000) to_read = 8000;
        int n = recv(Infos->Getfd(), buffer, to_read, MSG_DONTWAIT);
        if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            Infos->GetRequest().setBody(body);
            return;
        }
        if (n == 0) {
            const char *bad_request =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 45\r\n\r\n<html><body>400 Bad Request</body></html>";
            write(Infos->Getfd(), bad_request, strlen(bad_request));
            Infos->SetBool(true);
            return;
        }
        if (n < 0) {
            break;
        }
        body.append(buffer, n);
        total_read += n;
    }
    if (total_read < content_length) {
        Infos->GetRequest().setBody(body);
        return;
    }
    Infos->GetRequest().setBody(body);
    std::string upload_dir = matchedRoute.getUpload();
    if (!upload_dir.empty()) {
        std::string content_type = Infos->GetRequest().getHeader("Content-Type");
        std::string full_path;
        if (saveUploadedFile(upload_dir, content_type, body, full_path)) {
            std::string response_body = "File uploaded to: " + full_path + "\n";
            char header[256];
            sprintf(header,
                "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
                (unsigned long)response_body.size());
            std::string response = std::string(header) + response_body;
            write(Infos->Getfd(), response.c_str(), response.size());
            Infos->SetBool(true);
            return;
        }
    }
    std::string response_body = "POST received!\n";
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: text/plain\r\n"
        << "Content-Length: " << response_body.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << response_body;
    std::string response = oss.str();
    write(Infos->Getfd(), response.c_str(), response.size());
    Infos->SetBool(true);
}

static void handleDelete(Connection *Infos, Server *TmpServer, Route& matchedRoute) {
    std::string route = MatchRoutes(Infos->Getserver().getRoutes(), Infos->GetRequest());
    if (route == "404" || route == "405") {
        sendErrorResponse(Infos, TmpServer, atoi(route.c_str()));
        return;
    }
    std::string file_path = RemovePrefix(Infos->GetRequest().getRequestURI(), route, matchedRoute.getRoot());
    struct stat st;
    if (stat(file_path.c_str(), &st) != 0) {
        sendErrorResponse(Infos, TmpServer, 404);
        return;
    }
    if (unlink(file_path.c_str()) == 0) {
        std::ostringstream oss;
        oss << "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        std::string response = oss.str();
        write(Infos->Getfd(), response.c_str(), response.size());
        Infos->SetBool(true);
        return;
    } else {
        if (errno == EACCES || errno == EPERM) {
            sendErrorResponse(Infos, TmpServer, 403);
            return;
        } else {
            sendErrorResponse(Infos, TmpServer, 500);
            return;
        }
    }
}

// --- Main ResponseBuilder ---

HttpResponse::HttpResponse(int fd_client)
{
    this->fd_client = fd_client;
    offset = 0;
    this->status_code[200] = std::string("OK");
    this->status_code[301] = std::string("Moved Permanently");
    this->status_code[400] = std::string("Bad Request");
    this->status_code[403] = std::string("Forbidden");
    this->status_code[404] = std::string("Not Found");
    this->status_code[405] = std::string("Method Not Allowed");
    this->status_code[413] = std::string("Payload Too Large");
    this->status_code[500] = std::string("Internal Server Error");
}

void ResponseBuilder(Connection *Infos) {
    HttpResponse response(Infos->Getfd());
    Infos->SetHttpResponse(&response);

    // Parse cookies for all requests
    Infos->GetRequest().parseCookies();

    if (Infos->GetRequest().getMethod().empty()) {
        std::cerr << "[ERROR] Empty HTTP method received, sending 400 Bad Request" << std::endl;
        std::string bad_request =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 45\r\n"
            "\r\n"
            "<html><body>400 Bad Request</body></html>";
        write(Infos->Getfd(), bad_request.c_str(), bad_request.size());
        Infos->SetBool(true);
        return;
    }

    std::string host = Infos->GetRequest().getHeaders()["host"];
    Server *TmpServer =  &Infos->Getserver();
    const std::string url = Infos->GetRequest().getRequestURI();
    const size_t pos = url.find_last_of(".");
    Route &matchedRoute = Infos->Getserver().getRoutes()[MatchRoutes(Infos->Getserver().getRoutes(), Infos->GetRequest())];
    const bool isCGI = pos == std::string::npos ? false :  matchedRoute.getCGI().find(url.substr(pos + 1)) != matchedRoute.getCGI().end();
    if (HostName(&Infos->Getserver(), host) == false){
        sendErrorResponse(Infos, TmpServer, 400);
        return ;
    }
    else if (isCGI) {
        try {
            Cgi *cgi = Infos->getCGI();
            if (!cgi) {
                cgi = new Cgi(matchedRoute, Infos->GetRequest(), Infos);
                if (!cgi)
                    throw std::bad_alloc();
                Infos->SetCGI(cgi);
            }
            cgi->execute();
        }catch (const Cgi::CGIException &e) {
            sendErrorResponse(Infos, TmpServer, 500);
            return;
        }
    }

    // Enforce allowed methods per route
    std::vector<std::string> allowed_methods = matchedRoute.getMethods();
    std::string req_method = Infos->GetRequest().getMethod();
    bool method_allowed = false;
    for (size_t i = 0; i < allowed_methods.size(); ++i) {
        if (allowed_methods[i] == req_method) {
            method_allowed = true;
            break;
        }
    }
    if (!method_allowed) {
        send405(Infos, allowed_methods);
        return;
    }

    if (req_method == "GET") {
        handleGet(Infos);
        return;
    } else if (req_method == "POST") {
        handlePost(Infos, matchedRoute);
        return;
    } else if (req_method == "DELETE") {
        handleDelete(Infos, TmpServer, matchedRoute);
        return;
    }
    // else: no matching method branch
}

/*
	Author: BOUZID Hicham
	Description: check for all connections
				that found in our map connecions if done
	Date: 2025-04-23
*/

void MonitorConnection(std::map<int, Connection> *Connections,int epollFd){
	std::map<int , Connection>::iterator it= Connections->begin();
	struct  epoll_event event;

	for (; it != Connections->end(); it++){
		if (it->second.GetBool() == true)
		{
			if (epoll_ctl(epollFd, EPOLL_CTL_DEL, it->first, &event))
					std::cerr << "EPOLL_CTL_DEL: " << strerror(errno) << '\n';
			close(it->first);
			break;
		}
	}
	if (it != Connections->end())
	{
		// delete it->second.GetRequest;
		Connections->erase(it->first);
	}
}

std::string HttpResponse::GetStatusCode(int code_number){
	return status_code[code_number];
}

void HttpResponse::addSetCookie(const std::string& cookie) {
    set_cookies.push_back(cookie);
}
