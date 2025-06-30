#include "HttpResponse.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
HttpResponse::HttpResponse(int fd_client)
{
	// std::cout << "need to remove just for check Response builder\n";
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
	std::cout << "[DEBUG] Entered ResponseBuilder, method: '" << Infos->GetRequest().getMethod() << "'" << std::endl;
	HttpResponse response(Infos->Getfd());
	Infos->SetHttpResponse(&response);

    std::string version = Infos->GetRequest().getVersion();
    // if (version != "HTTP/1.1" && version != "HTTP/1.0") {
    //     // Hardcoded minimal HTTP/1.1 400 Bad Request response
    //     const char *bad_request =
    //         "HTTP/1.1 400 Bad Request\r\n"
    //         "Content-Type: text/html\r\n"
    //         "Content-Length: 45\r\n"
    //         "\r\n"
    //         "<html><body>400 Bad Request</body></html>";
    //     write(Infos->Getfd(), bad_request, strlen(bad_request));
    //     Infos->SetBool(true);
    //     return;
    // }

	std::string host = Infos->GetRequest().getHeaders()["Host"];
	Server *TmpServer =  &Infos->Getserver();
	const std::string url = Infos->GetRequest().getRequestURI();
	const size_t pos = url.find_last_of(".");
	Route &matchedRoute = Infos->Getserver().getRoutes()[MatchRoutes(Infos->Getserver().getRoutes(), Infos->GetRequest())];
	const bool isCGI = pos == std::string::npos ? false :  matchedRoute.getCGI().find(url.substr(pos + 1)) != matchedRoute.getCGI().end();
	if (HostName(&Infos->Getserver(), host) == false){
		std::cout << "[DEBUG] Entered HostName branch, method: '" << Infos->GetRequest().getMethod() << "'" << std::endl;
		write (Infos->Getfd(), ErrorBuilder(Infos, TmpServer, 400).c_str(), strlen(ErrorBuilder(Infos, TmpServer, 400).c_str()));
		Infos->SetBool(true);
		return ;
	}
	else if (isCGI) {
		std::cout << "[DEBUG] Entered CGI branch, method: '" << Infos->GetRequest().getMethod() << "'" << std::endl;
		try {
			Cgi *cgi = Infos->getCGI();

			if (!cgi) {
				std::cout << "pepe should be once\n";
				cgi = new Cgi(matchedRoute, Infos->GetRequest(), Infos);
				if (!cgi)
					throw std::bad_alloc();

				Infos->SetCGI(cgi);
			}
			cgi->execute();
		}catch (const Cgi::CGIException &e) {

			std::string tmp =  ErrorBuilder(Infos, TmpServer, 500);
			return;
		}
	}

	else if (Infos->GetRequest().getMethod() == "GET"){
		std::cout << "Client requested to : " << Infos->GetRequest().getRequestURI() << '\n';

		std::string tmpstring = GetMethod(Infos);
		if (!tmpstring.empty())
			write (Infos->Getfd(), tmpstring.c_str(), strlen(tmpstring.c_str()));
		return ;
	}
	else if (Infos->GetRequest().getMethod() == "POST"){
		std::cout << "[DEBUG] Method string: '" << Infos->GetRequest().getMethod() << "' length: " << Infos->GetRequest().getMethod().size() << std::endl;
		std::cout << "POST" << std::endl;

		// Read Content-Length header
		std::map<std::string, std::string> headers = Infos->GetRequest().getHeaders();
		std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
		if (it == headers.end()) {
			std::cerr << "No Content-Length header in POST request" << std::endl;
			const char *length_required =
				"HTTP/1.1 411 Length Required\r\nContent-Type: text/html\r\nContent-Length: 53\r\n\r\n<html><body>411 Length Required</body></html>";
			write(Infos->Getfd(), length_required, strlen(length_required));
			Infos->SetBool(true);
			return;
		}
		int content_length = atoi(it->second.c_str());
		if (content_length <= 0) {
			std::cerr << "Invalid Content-Length value in POST request" << std::endl;
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
				// Not enough data yet, save what we have and return
				Infos->GetRequest().setBody(body);
				std::cout << "[DEBUG] Not enough POST body data yet, read " << total_read << " of " << content_length << std::endl;
				return;
			}
			if (n == 0) {
				// Connection closed by client
				std::cerr << "[DEBUG] Connection closed by client, read " << total_read << " of " << content_length << std::endl;
				if (total_read < content_length) {
					// Incomplete upload, respond with 400
					const char *bad_request =
						"HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 45\r\n\r\n<html><body>400 Bad Request</body></html>";
					write(Infos->Getfd(), bad_request, strlen(bad_request));
					Infos->SetBool(true);
					return;
				}
				break;
			}
			if (n < 0) {
				std::cerr << "Error reading POST body from socket or connection closed" << std::endl;
				break;
			}
			body.append(buffer, n);
			total_read += n;
		}
		if (total_read < content_length) {
			// Still not enough data, wait for more
			std::cout << "[DEBUG] Still waiting for full POST body, have " << total_read << " of " << content_length << std::endl;
			Infos->GetRequest().setBody(body);
			return;
		}
		Infos->GetRequest().setBody(body);
		std::cout << "[DEBUG] POST body fully read: size=" << body.size() << std::endl;

		// Save uploaded file if upload directory is set
		std::string upload_dir = matchedRoute.getUpload();
		std::cout << "[DEBUG] Upload directory: " << upload_dir << std::endl;
		if (!upload_dir.empty()) {
			// MIME type to extension map
			std::map<std::string, std::string> mimeTypes;
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

			// Ensure directory exists (mkdir -p style, but only one level for C++98)
			struct stat st;
			if (stat(upload_dir.c_str(), &st) != 0) {
				if (mkdir(upload_dir.c_str(), 0777) != 0) {
					std::cerr << "[DEBUG] Failed to create upload directory: " << upload_dir << std::endl;
				}
			}
			// Get Content-Type header
			std::string content_type = Infos->GetRequest().getHeader("Content-Type");
			std::string ext = ".bin";
			if (mimeTypes.find(content_type) != mimeTypes.end()) {
				ext = mimeTypes[content_type];
			}
			// Generate a filename (upload_TIMESTAMP.ext)
			char filename[128];
			time_t now = time(0);
			sprintf(filename, "upload_%ld%s", (long)now, ext.c_str());
			std::string full_path = upload_dir + filename;
			std::ofstream outfile(full_path.c_str(), std::ios::binary);
			if (outfile.is_open()) {
				outfile.write(body.c_str(), body.size());
				outfile.close();
				std::cout << "[DEBUG] Uploaded file saved to: " << full_path << std::endl;
				// Respond with 201 Created
				std::string response_body = "File uploaded to: " + full_path + "\n";
				char header[256];
				sprintf(header,
					"HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
					(unsigned long)response_body.size());
				std::string response = std::string(header) + response_body;
				write(Infos->Getfd(), response.c_str(), response.size());
				Infos->SetBool(true);
				return;
			} else {
				std::cerr << "[DEBUG] Failed to open file for upload: " << full_path << std::endl;
			}
		}

		// Send a simple 200 OK response after reading the POST body
		std::cout << "[DEBUG] About to send 200 OK response for POST" << std::endl;
		std::string response_body = "POST received!\n";
		std::ostringstream oss;
		oss << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: text/plain\r\n"
			<< "Content-Length: " << response_body.size() << "\r\n"
			<< "Connection: close\r\n"
			<< "\r\n"
			<< response_body;
		std::string response = oss.str();
		int w = write(Infos->Getfd(), response.c_str(), response.size());
		std::cout << "[DEBUG] Sent 200 OK response for POST, write returned: " << w << std::endl;
		Infos->SetBool(true);
		std::cout << "[DEBUG] SetBool(true) called, returning from POST branch" << std::endl;
		return;
	}
	else if (Infos->GetRequest().getMethod() == "DELETE"){
		std::cout << "DELETE" << std::endl;
	}
	else {
		std::cout << "[DEBUG] No matching method branch, method: '" << Infos->GetRequest().getMethod() << "'" << std::endl;
	}
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
