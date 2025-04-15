#include "HttpResponse.hpp"

HttpResponse::HttpResponse(Connection &information)
{
	std::cout << "need to remove just for check Response builder\n";
	*infoResponse = information;
}

Connection &HttpResponse::GetRequest(){
	return *this->infoResponse;
}
