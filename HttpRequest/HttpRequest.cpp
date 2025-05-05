/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:02:21 by serraoui          #+#    #+#             */
/*   Updated: 2025/04/16 20:05:41 by hitchman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <iostream>

/*
    Constructors & Destructors
*/
HttpRequest::HttpRequest() {
    // BOUZID Hicham stop this constructor because it's
    // cause problems when i start to create an object Request.
    // Initialize the method handler map
    // _methodHandlerMap[HTTP_METHOD] = &HttpRequest::parseHttpMethod;
    // _methodHandlerMap[HTTP_REQUEST_URI] = &HttpRequest::parseHttpRequestUriMethod;
    // _methodHandlerMap[HTTP_VERSION] = &HttpRequest::parseHttpVersionMethod;
    // _methodHandlerMap[HTTP_HEADERS] = &HttpRequest::parseHttpHeadersMethod;
    // _methodHandlerMap[HTTP_BODY] = &HttpRequest::parseHttpBodyMethod;
    // _state = HTTP_METHOD;
}

HttpRequest::~HttpRequest() {}

/*
    Getters
*/
std::string     HttpRequest::getMethod() const {
    return _method;
}

std::string     HttpRequest::getRequestURI() const {
    return _requestURI;
}

std::string     HttpRequest::getVersion() const {
    return _version;
}

std::string     HttpRequest::getBody() const {
    return _body;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const {
    return _headers;
}

int             HttpRequest::getState() const {
    return _state;
}

/*
    Setters
*/
void            HttpRequest::setMethod(std::string method) {
    this->_method = method;
}

void            HttpRequest::setRequestURI(std::string requestURI) {
    this->_requestURI = requestURI;
}

void            HttpRequest::setVersion(std::string version) {
    this->_version = version;
}

void            HttpRequest::setBodyFd(int bodyFd) {
    this->_body = bodyFd;
}

/*
    Member methods
*/
void HttpRequest::parseHttpMethod(char byte) {
    // std::cout << "parseHttpMethod() " << std::endl;

    if (byte == ' ' || byte == '\n') {
        if (_method == "GET" || _method == "POST" || _method == "DELETE") {
            setMethod(_method);
            //Todo : add success handler
            _state = HTTP_REQUEST_URI; //next state
            std::cout << "Parsed HTTP Method: " << _method << std::endl;
        } else {
            std::cerr << "Invalid HTTP Method: " << _method << std::endl;
            //Todo : add error handler
            exit(1);
        }
    } else {
        _method += byte;
    }
}

void HttpRequest::parseHttpRequestUriMethod(char byte) {
    // std::cout << "parseHttpRequestUriMethod() " << std::endl;

    if (byte == ' ' || byte == '\n') {
        if (!_requestURI.empty()) {
            //Todo : check if the request URI equals * or abspath
            setRequestURI(_requestURI);
            // Move to the next state, e.g., HTTP_VERSION
            _state = HTTP_VERSION;
            std::cout << "Parsed HTTP Request URI: " << _requestURI << std::endl;
        } else {
            std::cerr << "Invalid HTTP Request URI" << std::endl;
            // Handle error
            exit(1);
        }
        _requestURI.clear(); // Reset for next parsing
    } else {
        _requestURI += byte;
    }
}

void HttpRequest::parseHttpVersionMethod(char byte) {
    // std::cout << "parseHttpVersionMethod() " << std::endl;

    if (byte == '\n') {
        if (_version.find("HTTP/") == 0) {
            setVersion(_version);
            // Move to the next state, e.g., HTTP_HEADERS
            _state = HTTP_HEADERS;
            std::cout << "Parsed HTTP Version: " << _version << std::endl;
        } else {
            std::cerr << "Invalid HTTP Version: " << _version << std::endl;
            // Handle error
            exit(1);
        }
        // _version.clear(); // Reset for next parsing
    } else {
        _version += byte;
    }
}

void HttpRequest::parseHttpHeadersMethod(char byte) {
    static bool lastCharWasCR = false; // Track if the last character was a carriage return
    static std::string _accumulatedHeader; // Track if the last character was a carriage return

    // std::cout << "parseHttpHeadersMethod() " << std::endl;
    if (byte == '\n' && lastCharWasCR) {
        // End of headers section
        _state = HTTP_BODY;
        std::cout << "Parsed HTTP Headers: " << std::endl;
        _accumulatedHeader.clear();
        lastCharWasCR = false;
        return;
    }

    if (byte == '\n') {
        // End of a header line
        size_t delimiterPos = _accumulatedHeader.find(':');
        if (delimiterPos != std::string::npos) {
            std::string key = _accumulatedHeader.substr(0, delimiterPos);
            std::string value = _accumulatedHeader.substr(delimiterPos + 1);
            _headers[key] = value;
        }
        _accumulatedHeader.clear();
    } else if (byte != '\r') {
        _accumulatedHeader += byte;
    }
    std::cout << "BYTE >> "  << byte << " ASCII: " << (int)byte << "; to compare: 13;"<< std::endl;
    lastCharWasCR = (byte == '\r');
}

void HttpRequest::parseHttpBodyMethod(char byte) {
    // std::cout << "parseHttpBodyMethod() " << std::endl;

    // Accumulate body content
    _body += byte;

    // For demonstration, let's assume we print the body when done
    // In a real scenario, you might check for content length or other termination conditions
    std::cout << "Accumulating HTTP Body: " << _body << std::endl;
}

void HttpRequest::parseHttpRequestOrchestrator(char byte) {
    switch (_state) {
        case HTTP_METHOD:
            parseHttpMethod(byte);
            break;
        case HTTP_REQUEST_URI:
            parseHttpRequestUriMethod(byte);
            break;
        case HTTP_VERSION:
            parseHttpVersionMethod(byte);
            break;
        case HTTP_HEADERS:
            parseHttpHeadersMethod(byte);
            break;
        case HTTP_BODY:
            parseHttpBodyMethod(byte);
            break;
        default:
            std::cerr << "Unknown state: " << _state << std::endl;
            break;
    }
}

void HttpRequest::setHeaders(std::string key, std::string value){
    this->_headers[key] = value;
}


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
        this->_methodHandlerMap = copy_HttpRequest._methodHandlerMap;
    }
    return (*this);
}


void HttpRequest::ClearURI(){
    this->_requestURI = "";
}
