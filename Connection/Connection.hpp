#pragma once

#include "server.hpp"
#include "../HttpRequest/HttpRequest.hpp"
// class connection containe the fd socket and class severe and
class HttpRequest;
class HttpResponse;
class Server;

/*
	Author: BOUZID Hicham
	Discreption: Class to handle connection btween sever & client
*/

class Connection{
	// object request
		HttpRequest		*Request;
		HttpResponse	*Response;
		Server			*s;
		std::string		path_optional;
		int				fd_client;
		bool			done;
		long			timeout;
	public:
		Connection(int FdServer, int FdEpoll,WebServ *MainObject);
		Connection();
		void 		SetHttpRequest(HttpRequest *Request);
		void		SetHttpRespons(HttpResponse *Response);
		void		SetBool(bool);

		int				Getfd();
		bool			GetBool();
		HttpRequest&	GetRequest();
		HttpResponse&	GetResponse();
		Server&			Getserver();
};
