#include "HttpRequest.hpp"

/*
    Constructors & Destructors
*/
HttpRequest() {}

~HttpRequest() {}


/*
    Getters
*/
std::string     getMethod() const {
    return _method;
}

std::string     getRequestURI() const {
    return _requestURI;
}

std::string     getVersion() const {
    return _version;
}

int             getBodyFd() const {
    return _body;
}


/*
    Setters
*/
void            setMethod(std::string method) {
    this->_method = method;
}

void            setRequestURI(std::string requestURI) {
    this->_requestURI = requestURI;
}

void            setVersion(std::string version) {
    this->_version = version;
}

void            setBodyFd(int bodyFd) {
    this->_body = bodyFd;
}


/*
    Member methods
*/
