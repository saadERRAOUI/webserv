#pragma once

#include "server.hpp"
#include "../HttpRequest/HttpRequest.hpp"
// class connection containe the fd socket and class severe and
class HttpRequest;
class Server;

/*
	Author: BOUZID Hicham
	Discreption: Class to handle connection btween sever & client
*/

class Connection{
	// object request
		HttpRequest *Request;
		Server *s;
		int fd_client;
		bool done;
		long timeout;
	public:
		Connection(int FdServer, int FdEpoll,WebServ *MainObject);
		Connection();
		void SetHttpRequest(HttpRequest &Request);
		int Getfd();
};
