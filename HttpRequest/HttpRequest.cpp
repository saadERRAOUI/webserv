/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sahazel <sahazel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:02:21 by serraoui          #+#    #+#             */
/*   Updated: 2025/07/04 22:35:20 by sahazel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <cctype>
#include <string>

// Declaration for to_lower utility
std::string to_lower(const std::string& s);

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

bool  HttpRequest::getIsCGI()
{
    return this->isCGI;
}


int             HttpRequest::getState() const {return _state;}

std::string     HttpRequest::getHeaderKey() const {return _headerKey;}

std::string     HttpRequest::getHeaderValue() const {return _headerValue;}

int             HttpRequest::getEndSequenceState() const {return _endSequenceState;}

bool            HttpRequest::getIsChunked() const {return _isChunked;}

std::string     HttpRequest::getQueryString() const {return _queryString;}

std::string     HttpRequest::getFragment() const {return _fragment;}

void HttpRequest::setIsCGI(bool isCGI)
{
    this->isCGI = isCGI;
}
std::map<std::string, std::string> HttpRequest::getQueryParams() const {
    std::map<std::string, std::string> queryParams;
    size_t pos = _requestURI.find('?');
    if (pos != std::string::npos) {
        std::string queryString = _requestURI.substr(pos + 1);
        size_t start = 0;
        size_t end = queryString.find('&');
        while (end != std::string::npos) {
            std::string param = queryString.substr(start, end - start);
            size_t equalPos = param.find('=');
            if (equalPos != std::string::npos) {
                std::string key = param.substr(0, equalPos);
                std::string value = param.substr(equalPos + 1);
                queryParams[key] = value;
            }
            start = end + 1;
            end = queryString.find('&', start);
        }
        // Handle the last parameter
        std::string param = queryString.substr(start);
        size_t equalPos = param.find('=');
        if (equalPos != std::string::npos) {
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            queryParams[key] = value;
        }
    }
    return queryParams;
}

std::string     HttpRequest::getHeader(std::string key) const {
    std::string key_lower = to_lower(key);
    std::map<std::string, std::string>::const_iterator it = _headers.find(key_lower);
    std::cout << "[getHeader] lookup: '" << key << "' (as '" << key_lower << "') => ";
    if (it != _headers.end()) {
        std::cout << "found: '" << it->second << "'\n";
        return it->second;
    }
    std::cout << "not found\n";
    return "";
}
/*
    Setters
*/
void            HttpRequest::setMethod(std::string method) {this->_method = method;}

void            HttpRequest::setRequestURI(std::string requestURI) {this->_requestURI = requestURI;}

void            HttpRequest::setVersion(std::string version) {this->_version = version;}

void            HttpRequest::setBodyFd(int bodyFd) {this->_body = bodyFd;}

void            HttpRequest::setHeaders(std::string headerKey, std::string headerValue) {
    std::string key_lower = to_lower(headerKey);
    std::cout << "[setHeaders] key: '" << headerKey << "' (stored as '" << key_lower << "'), value: '" << headerValue << "'\n";
    this->_headers.insert(std::make_pair(key_lower, headerValue));
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

void HttpRequest::parseCookies() {
    _cookies.clear();
    std::string cookieHeader = getHeader("Cookie");
    size_t pos = 0;
    while (pos < cookieHeader.size()) {
        size_t eq = cookieHeader.find('=', pos);
        if (eq == std::string::npos) break;
        size_t sc = cookieHeader.find(';', eq);
        std::string name = cookieHeader.substr(pos, eq - pos);
        std::string value = (sc == std::string::npos) ? cookieHeader.substr(eq + 1) : cookieHeader.substr(eq + 1, sc - eq - 1);
        // Trim whitespace
        while (!name.empty() && (name[0] == ' ' || name[0] == '\t')) name.erase(0, 1);
        while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
        _cookies[name] = value;
        if (sc == std::string::npos) break;
        pos = sc + 1;
    }
}

std::string HttpRequest::getCookie(const std::string& name) const {
    std::map<std::string, std::string>::const_iterator it = _cookies.find(name);
    if (it != _cookies.end()) return it->second;
    return "";
}
