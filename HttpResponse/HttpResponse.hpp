#pragma once
#include "../HttpRequest/HttpRequest.hpp"
#include "../Connection/Connection.hpp"


/*
	Author: BOUZID Hicham
	Description:  build response depending
					on the request parsing
	Date: 2025-04-15
*/
// class HttpRequest ;
class Connection;
class HttpResponse
{
	Connection *infoResponse;
	public:
		HttpResponse(Connection &);
		Connection &GetRequest();
};
