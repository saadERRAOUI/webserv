/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:02:21 by serraoui          #+#    #+#             */
/*   Updated: 2025/05/10 15:13:21 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/*
    Constructors & Destructors
*/
HttpRequest::HttpRequest() {
    _state = HTTP_METHOD_START;
    _endSequenceState = 0;
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

void            HttpRequest::setBody(std::string body) {this->_body = body;}