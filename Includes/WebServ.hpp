#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//Include std libraries
//Add Macros
//Add Constants

#include <iostream>
#include <string>
#include <algorithm>
#include <map>

enum HttpRequestState {
    HTTP_METHOD,
    HTTP_REQUEST_URI,
    HTTP_VERSION,
    HTTP_HEADERS,
    HTTP_BODY,
    HTTP_COMPLETE,
    HTTP_ERROR
};



#endif WEBSERV_HPP
