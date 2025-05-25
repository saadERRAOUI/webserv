/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:45:32 by serraoui          #+#    #+#             */
/*   Updated: 2025/05/25 17:21:45 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {
    _methodHandlerMap[HTTP_METHOD_START] = &HttpRequestParser::parseHttpMethod;
    _methodHandlerMap[HTTP_METHOD] = &HttpRequestParser::parseHttpMethod;
    _methodHandlerMap[HTTP_METHOD_SP] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_REQUEST_URI] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_QUERY_STRING_START] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_QUERY_STRING] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_FRAGMENT_START] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_FRAGMENT] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_REQUEST_URI_SP] = &HttpRequestParser::parseHttpRequestUriMethod;
    _methodHandlerMap[HTTP_VERSION] = &HttpRequestParser::parseHttpVersionMethod;
    _methodHandlerMap[HTTP_VERSION_CR] = &HttpRequestParser::parseHttpVersionMethod;
    _methodHandlerMap[HTTP_HEADER_START] = &HttpRequestParser::parseHttpHeadersMethod;
    _methodHandlerMap[HTTP_HEADER_KEY] = &HttpRequestParser::parseHttpHeadersMethod;
    _methodHandlerMap[HTTP_HEADER_SP] = &HttpRequestParser::parseHttpHeadersMethod;
    _methodHandlerMap[HTTP_HEADER_VAL] = &HttpRequestParser::parseHttpHeadersMethod;
    _methodHandlerMap[HTTP_HEADER_NL] = &HttpRequestParser::parseHttpHeadersMethod;
    _methodHandlerMap[HTTP_BODY_START] = &HttpRequestParser::parseHttpBodyMethod;
    _methodHandlerMap[HTTP_BODY] = &HttpRequestParser::parseHttpBodyMethod;
}

HttpRequestParser::~HttpRequestParser() {}

/*
    utility methods
*/
bool	HttpRequestParser::isChar(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool	HttpRequestParser::isControl(char c) const {
    return (c >= 0 && c <= 31) || (c == 127);
}

bool	HttpRequestParser::isSpecial(char c) const {
    return  (c == '(' || c == ')' || c == '<' || 
            c == '>' || c == '@' || c == ',' ||
            c == ';' || c == ':' || c == '\\' || 
            c == '"' || c == '/' || c == '[' || 
            c == ']' || c == '?' || c == '=' ||
            c == '{' || c == '}' || c == ' ' || c == '\t');
}

bool    HttpRequestParser::isValidURIChar(char c) const {
    return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' ||
           c == '/' || c == ':' || c == '@' || c == '!' || c == '$' ||
           c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' ||
           c == '+' || c == ',' || c == ';' || c == '=' || c == '%';
}

bool	HttpRequestParser::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

/*
    Parser Member methods
*/
ParseResult HttpRequestParser::parseHttpMethod(HttpRequest& request, char byte) {
    switch (request.getState()) {
        case HTTP_METHOD_START:
            if (!isChar(byte) || isControl(byte) || isSpecial(byte)) {
                return PARSE_ERROR;
            }
            else if (isChar(byte)) {
                request.setState(HTTP_METHOD);
                request.setMethod(request.getMethod() + byte);
            }
            break;
        case HTTP_METHOD:
            if (byte == ' ') {
                request.setState(HTTP_METHOD_SP);
            } else if (!isChar(byte) || isControl(byte) || isSpecial(byte)) {
                return PARSE_ERROR;
            } else {
                request.setMethod(request.getMethod() + byte);
            }
            break;   
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpRequestUriMethod(HttpRequest& request, char byte) {
    switch (request.getState()) {
        case HTTP_METHOD_SP:
            if (!isChar(byte) && byte != '/')
                return PARSE_ERROR;
            else {
                request.setRequestURI(request.getRequestURI() + byte);
                request.setState(HTTP_REQUEST_URI);
            }
            break;
        case HTTP_REQUEST_URI:
            std::cout << "Request URI >> " << byte << std::endl;
            if (byte == ' ')
                request.setState(HTTP_REQUEST_URI_SP);
            else if (byte == '?')
                request.setState(HTTP_QUERY_STRING_START);
            else if (byte == '#'){
                std::cout << "Fragment start >> "  << byte << std::endl;
                request.setState(HTTP_FRAGMENT_START);
            }
            else if (!isValidURIChar(byte))
                return PARSE_ERROR;
            else
                request.setRequestURI(request.getRequestURI() + byte);
            break;
        case HTTP_REQUEST_URI_SP:
            if (byte == 'H') {
                request.setVersion(request.getVersion() + byte);
                request.setState(HTTP_VERSION);
            } else
                return PARSE_ERROR;
            break;

        /*
            Todo : Implement fragment and query string states
        */
        case HTTP_QUERY_STRING_START:
            if (isControl(byte))
                return PARSE_ERROR;
            else if (byte == '#')
                request.setState(HTTP_FRAGMENT_START);
            else {
                request.setState(HTTP_QUERY_STRING);
                request.setQueryString(request.getQueryString() + byte);
            }
            break;
        case HTTP_QUERY_STRING:
            if (byte == ' ')
                request.setState(HTTP_REQUEST_URI_SP);
            else if (byte == '#')
                request.setState(HTTP_FRAGMENT_START);
            else if (isControl(byte))
                return PARSE_ERROR;
            else
                request.setQueryString(request.getQueryString() + byte);
            break;
        case HTTP_FRAGMENT_START:
            std::cout << "Fragment start" << std::endl;
            if (isControl(byte))
                return PARSE_ERROR;
            else {
                request.setState(HTTP_FRAGMENT);
                request.setFragment(request.getFragment() + byte);
            }
            break;
        case HTTP_FRAGMENT:
            if (byte == ' ')
                request.setState(HTTP_REQUEST_URI_SP);
            else if (isControl(byte))
                return PARSE_ERROR;
            else
                request.setFragment(request.getFragment() + byte);
            break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpVersionMethod(HttpRequest& request, char byte) {
    static std::string version = "HTTP/1.0";

    switch (request.getState()) {
        case HTTP_VERSION:
            if (byte == '\r') {
                request.setState(HTTP_VERSION_CR);
            } else if (version.find(byte) != std::string::npos) {
                request.setVersion(request.getVersion() + byte);
            } else {
                return PARSE_ERROR;
            }
            break;
        case HTTP_VERSION_CR:
            if (byte == '\n') {
                request.setState(HTTP_HEADER_START);
            } else {
                return PARSE_ERROR;
            }
            break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpHeadersMethod(HttpRequest& request, char byte) {
    switch (request.getState()) {
        case HTTP_HEADER_START:
            if (byte == '\r') {
                request.setState(HTTP_BODY_START);
            } else if (!std::isalnum(byte) && byte != '-' && byte != '_')
                return PARSE_ERROR;
            else {
                request.setHeaderKey(request.getHeaderKey() + byte);
                request.setState(HTTP_HEADER_KEY);
            }
            break;
        
        case HTTP_HEADER_KEY:
            if (byte == ':') {
                request.setState(HTTP_HEADER_SP);
            } else if (!std::isalnum(byte) && byte != '-' && byte != '_') {
                return PARSE_ERROR;
            } else {
                request.setHeaderKey(request.getHeaderKey() + byte);
            }
            break;

        case HTTP_HEADER_SP:
            if (byte == ' ') {
                request.setState(HTTP_HEADER_VAL);
            } else {
                return PARSE_ERROR;
            }
            break;

        case HTTP_HEADER_VAL:
            if (byte == '\r') {
                request.setState(HTTP_HEADER_NL);
            } else if (isControl(byte)) {
                return PARSE_ERROR;
            } else {
                request.setHeaderValue(request.getHeaderValue() + byte);
            }
            break;

        case HTTP_HEADER_NL:
            if (byte == '\n') {
                request.setState(HTTP_HEADER_START);
                /*
                    Manage chunked request 
                */
                if (
                    request.getMethod() == "POST" &&
                    request.getHeaderKey() == "Transfer-encoding" &&
                    request.getHeaderValue() == "chuncked"
                )
                    request.setIsChunked(true);
                request.setHeaders(request.getHeaderKey(), request.getHeaderValue());
                request.setHeaderKey("");
                request.setHeaderValue("");
            }
            else 
                return PARSE_ERROR;
            break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpBodyMethod(HttpRequest& request, char byte) {
    switch (request.getState()) {
        case HTTP_BODY_START:
            if (byte == '\n') {
                request.setState(HTTP_BODY);
            } else {
                return PARSE_ERROR;
            }
            break;
        
        case HTTP_BODY:
            if (byte == '\r') {
                return PARSE_INPROGRESS;
            } else {
                request.setBody(request.getBody() + byte);
            }
            break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parse(HttpRequest& request, char buffer[], int size) {
    ParseResult result = PARSE_START;

    for (int i = 0; i < size; i++) {
        std::map<HttpRequestState, ParseResult (HttpRequestParser::*)(HttpRequest&, char)>::iterator methodHandler = 
            _methodHandlerMap.find((HttpRequestState)request.getState());
        
        if (methodHandler != _methodHandlerMap.end()) {
            result = (this->*methodHandler->second)(request, buffer[i]);
            if (result == PARSE_ERROR)
                return PARSE_ERROR;
        } else {
            std::cerr << "Unknown state: " << request.getState() << std::endl;
            return PARSE_ERROR;
        }
    }
    return result;
}

std::string HttpRequestParser::getStateName(HttpRequestState state) const {
    switch (state) {
        case HTTP_METHOD_START: return "HTTP_METHOD_START";
        case HTTP_METHOD: return "HTTP_METHOD";
        case HTTP_METHOD_SP: return "HTTP_METHOD_SP";
        case HTTP_REQUEST_URI: return "HTTP_REQUEST_URI";
        case HTTP_REQUEST_URI_SP: return "HTTP_REQUEST_URI_SP";
        case HTTP_VERSION: return "HTTP_VERSION";
        case HTTP_VERSION_CR: return "HTTP_VERSION_CR";
        case HTTP_HEADER_START: return "HTTP_HEADER_START";
        case HTTP_HEADER_KEY: return "HTTP_HEADER_KEY";
        case HTTP_HEADER_SP: return "HTTP_HEADER_SP";
        case HTTP_HEADER_VAL: return "HTTP_HEADER_VAL";
        case HTTP_HEADER_NL: return "HTTP_HEADER_NL";
        case HTTP_BODY_START: return "HTTP_BODY_START";
        case HTTP_BODY: return "HTTP_BODY";
        case HTTP_PARSE_ERROR: return "HTTP_PARSE_ERROR";
        default: return "UNKNOWN_STATE";
    }
}