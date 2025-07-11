#pragma once

#include "server.hpp"
#include "Cgi.hpp"
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
		Cgi 			*CGI;
		Server			*s;
		std::string		path_optional;
		int				fd_client;
		bool			done;
		int				size;
		long			timeout;
		std::ifstream 	*file;
		// vector to prevent from infinit redirection
		std::vector<std::string> redirect;
	public:
		Connection(int FdServer, int FdEpoll,WebServ *MainObject);
		Connection();
		~Connection();
		void 			SetHttpRequest(HttpRequest *Request);
		void			SetHttpResponse(HttpResponse *Response);
		void			SetBool(bool);
		bool			SetRedirect(std::string URI);
		void			SetSize(int);
		void			Setfile(std::ifstream &file);
		void            SetCGI(Cgi *);
		Cgi				*getCGI();
		int				Getfd();
		bool			GetBool();
		HttpRequest&	GetRequest();
		HttpResponse&	GetResponse();
		Server&			Getserver();
		int				GetSize();
		void			DefSize(int);
		std::ifstream	*GetFile();
		void			ChagenMode(int FdEpoll, int fd_client, int mood);
		long			GetTimeout() const;
		void			SetTimeout(long timeout);

};

// Function declaration for timeout functionality
long get_current_time(void);
