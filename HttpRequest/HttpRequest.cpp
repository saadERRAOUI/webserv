/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:02:21 by serraoui          #+#    #+#             */
/*   Updated: 2025/05/25 19:12:03 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/*
    Constructors & Destructors
*/
HttpRequest::HttpRequest() {
    _state = HTTP_METHOD_START;
    _endSequenceState = 0;
    _isChunked = false;
}

HttpRequest::~HttpRequest() {}

/*
    Getters
*/
std::string     HttpRequest::getMethod() const {return _method;}

std::string     HttpRequest::getRequestURI() const {return _requestURI;}

std::string     HttpRequest::getVersion() const {return _version;}

std::string     HttpRequest::getBody() const {return _body;}

std::map<std::string, std::string> HttpRequest::getHeaders() const {return _headers;}

int             HttpRequest::getState() const {return _state;}

std::string     HttpRequest::getHeaderKey() const {return _headerKey;}

std::string     HttpRequest::getHeaderValue() const {return _headerValue;}

int             HttpRequest::getEndSequenceState() const {return _endSequenceState;}

bool            HttpRequest::getIsChunked() const {return _isChunked;}

std::string     HttpRequest::getQueryString() const {return _queryString;}

std::string     HttpRequest::getFragment() const {return _fragment;}

/*
    Setters
*/
void            HttpRequest::setMethod(std::string method) {this->_method = method;}

void            HttpRequest::setRequestURI(std::string requestURI) {this->_requestURI = requestURI;}

void            HttpRequest::setVersion(std::string version) {this->_version = version;}

void            HttpRequest::setBodyFd(int bodyFd) {this->_body = bodyFd;}

void            HttpRequest::setHeaders(std::string headerKey, std::string headerValue) {
    this->_headers.insert(std::make_pair(headerKey, headerValue));
}

void            HttpRequest::setHeaderKey(std::string headerKey) {this->_headerKey = headerKey;}

void            HttpRequest::setHeaderValue(std::string headerValue) {this->_headerValue = headerValue;}

void            HttpRequest::setEndSequenceState(int endSequenceState) {this->_endSequenceState = endSequenceState;}

void            HttpRequest::setState(int state) {this->_state = state;}

void            HttpRequest::setIsChunked(bool isChunked) {this->_isChunked = isChunked;}

void            HttpRequest::setQueryString(std::string queryString) {this->_queryString = queryString;}

void            HttpRequest::setFragment(std::string fragment) {this->_fragment = fragment;}

/*
    Author: BOUZID Hicham
    Description: copy assignment operator
    date: 2025-04-16
*/
HttpRequest &HttpRequest::operator=(const HttpRequest &copy_HttpRequest){
    if (this != &copy_HttpRequest)
    {
        this->_method = copy_HttpRequest._method;
        this->_requestURI = copy_HttpRequest._requestURI;
        this->_version = copy_HttpRequest._version;
        this->_body = copy_HttpRequest._body;
        this->_headers = copy_HttpRequest._headers;
        this->_state = copy_HttpRequest._state;
        // this->_methodHandlerMap = copy_HttpRequest._methodHandlerMap;
    }
    return (*this);
}


void HttpRequest::ClearURI(){
    this->_version = "";
}
void            HttpRequest::setBody(std::string body) {this->_body = body;}

void            HttpRequest::showRequest() const {
    std::cout << "Method: " << _method << std::endl;
    std::cout << "Request URI: " << _requestURI << std::endl;
    std::cout << "Version: " << _version << std::endl;
    std::cout << "Headers:" << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "IsChunked: " << _isChunked << std::endl;
    std::cout << "Query String: " << _queryString << std::endl;
    std::cout << "Fragment: " << _fragment << std::endl;
    std::cout << "Body: " << _body << std::endl;
    std::cout << "Body size: " << _body.length() << std::endl;
    std::cout << "State: " << _state << std::endl;
}
