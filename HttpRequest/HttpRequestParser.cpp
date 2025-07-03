/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sahazel <sahazel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:45:32 by serraoui          #+#    #+#             */
/*   Updated: 2025/07/03 13:16:27 by sahazel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"

// Helper to convert a string to lowercase
std::string to_lower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); ++i) out[i] = std::tolower(out[i]);
    return out;
}

HttpRequestParser::HttpRequestParser()
{
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
bool HttpRequestParser::isChar(char c) const
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool HttpRequestParser::isControl(char c) const
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool HttpRequestParser::isSpecial(char c) const
{
    return (c == '(' || c == ')' || c == '<' ||
            c == '>' || c == '@' || c == ',' ||
            c == ';' || c == ':' || c == '\\' ||
            c == '"' || c == '/' || c == '[' ||
            c == ']' || c == '?' || c == '=' ||
            c == '{' || c == '}' || c == ' ' || c == '\t');
}

bool HttpRequestParser::isValidURIChar(char c) const
{
    return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' ||
           c == '/' || c == ':' || c == '@' || c == '!' || c == '$' ||
           c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' ||
           c == '+' || c == ',' || c == ';' || c == '=' || c == '%';
}

bool HttpRequestParser::isDigit(char c) const
{
    return c >= '0' && c <= '9';
}

/*
    Parser Member methods
*/
ParseResult HttpRequestParser::parseHttpMethod(HttpRequest &request, char byte)
{
    switch (request.getState())
    {
    case HTTP_METHOD_START:
        if (!isChar(byte) || isControl(byte) || isSpecial(byte))
        {
            return PARSE_ERROR;
        }
        else if (isChar(byte))
        {
            request.setState(HTTP_METHOD);
            request.setMethod(request.getMethod() + byte);
        }
        break;
    case HTTP_METHOD:
        if (byte == ' ')
        {
            request.setState(HTTP_METHOD_SP);
        }
        else if (!isChar(byte) || isControl(byte) || isSpecial(byte))
        {
            return PARSE_ERROR;
        }
        else
        {
            request.setMethod(request.getMethod() + byte);
        }
        break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpRequestUriMethod(HttpRequest &request, char byte)
{
    switch (request.getState())
    {
    case HTTP_METHOD_SP:
        if (!isChar(byte) && byte != '/')
            return PARSE_ERROR;
        else
        {
            request.setRequestURI(request.getRequestURI() + byte);
            request.setState(HTTP_REQUEST_URI);
        }
        break;
    case HTTP_REQUEST_URI:
        if (byte == ' ')
            request.setState(HTTP_REQUEST_URI_SP);
        else if (byte == '?')
            request.setState(HTTP_QUERY_STRING_START);
        else if (byte == '#')
            request.setState(HTTP_FRAGMENT_START);
        else if (!isValidURIChar(byte))
            return PARSE_ERROR;
        else
            request.setRequestURI(request.getRequestURI() + byte);
        break;
    case HTTP_REQUEST_URI_SP:
        if (byte == 'H')
        {
            request.setVersion(request.getVersion() + byte);
            request.setState(HTTP_VERSION);
        }
        else
            return PARSE_ERROR;
        break;
    case HTTP_QUERY_STRING_START:
        if (isControl(byte))
            return PARSE_ERROR;
        else if (byte == '#')
            request.setState(HTTP_FRAGMENT_START);
        else
        {
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
        if (isControl(byte))
            return PARSE_ERROR;
        else
        {
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

ParseResult HttpRequestParser::parseHttpVersionMethod(HttpRequest &request, char byte)
{
    static std::string version = "HTTP/1.0";

    switch (request.getState())
    {
    case HTTP_VERSION:
        if (byte == '\r')
        {
            request.setState(HTTP_VERSION_CR);
        }
        else if (version.find(byte) != std::string::npos)
        {
            request.setVersion(request.getVersion() + byte);
        }
        else
        {
            return PARSE_ERROR;
        }
        break;
    case HTTP_VERSION_CR:
        if (byte == '\n')
        {
            request.setState(HTTP_HEADER_START);
        }
        else
        {
            return PARSE_ERROR;
        }
        break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpHeadersMethod(HttpRequest &request, char byte)
{
    switch (request.getState())
    {
    case HTTP_HEADER_START:
        if (byte == '\r')
        {
            request.setState(HTTP_BODY_START);
        }
        else if (!std::isalnum(byte) && byte != '-' && byte != '_')
            return PARSE_ERROR;
        else
        {
            request.setHeaderKey(request.getHeaderKey() + byte);
            request.setState(HTTP_HEADER_KEY);
        }
        break;

    case HTTP_HEADER_KEY:
        if (byte == ':')
        {
            request.setState(HTTP_HEADER_SP);
        }
        else if (!std::isalnum(byte) && byte != '-' && byte != '_')
        {
            return PARSE_ERROR;
        }
        else
        {
            request.setHeaderKey(request.getHeaderKey() + byte);
        }
        break;

    case HTTP_HEADER_SP:
        if (byte == ' ')
        {
            request.setState(HTTP_HEADER_VAL);
        }
        else
        {
            return PARSE_ERROR;
        }
        break;

    case HTTP_HEADER_VAL:
        if (byte == '\r')
        {
            request.setState(HTTP_HEADER_NL);
        }
        else if (isControl(byte))
        {
            return PARSE_ERROR;
        }
        else
        {
            request.setHeaderValue(request.getHeaderValue() + byte);
        }
        break;

    case HTTP_HEADER_NL:
        if (byte == '\n')
        {
            request.setState(HTTP_HEADER_START);
            std::string key_lower = to_lower(request.getHeaderKey());
            std::string val_lower = to_lower(request.getHeaderValue());
            request.setHeaders(key_lower, request.getHeaderValue());
            if (key_lower == "transfer-encoding" && val_lower.find("chunked") != std::string::npos)
                request.setIsChunked(true);
            request.setHeaderKey("");
            request.setHeaderValue("");
        }
        else
            return PARSE_ERROR;
        break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parseHttpBodyMethod(HttpRequest &request, char byte)
{
    switch (request.getState())
    {
    case HTTP_BODY_START:
        if (byte == '\n')
        {
            request.setState(HTTP_BODY);
        }
        else
        {
            return PARSE_ERROR;
        }
        break;

    case HTTP_BODY:
        // Continue reading body data, including \r characters
        request.setBody(request.getBody() + byte);
        break;
    }
    return PARSE_INPROGRESS;
}

ParseResult HttpRequestParser::parse(HttpRequest &request, char buffer[], int size)
{
    ParseResult result = PARSE_START;

    for (int i = 0; i < size; i++)
    {
        std::map<HttpRequestState, ParseResult (HttpRequestParser::*)(HttpRequest &, char)>::iterator methodHandler =
            _methodHandlerMap.find((HttpRequestState)request.getState());

        if (methodHandler != _methodHandlerMap.end())
        {
            result = (this->*methodHandler->second)(request, buffer[i]);
            if (result == PARSE_ERROR)
                return PARSE_ERROR;
        }
        else
        {
            std::cerr << "Unknown state: " << request.getState() << std::endl;
            return PARSE_ERROR;
        }
    }
    if (size < BUFFER_SIZE)
        return PARSE_SUCCESS;
    return result;
}

std::string HttpRequestParser::getStateName(HttpRequestState state) const
{
    switch (state)
    {
    case HTTP_METHOD_START:
        return "HTTP_METHOD_START";
    case HTTP_METHOD:
        return "HTTP_METHOD";
    case HTTP_METHOD_SP:
        return "HTTP_METHOD_SP";
    case HTTP_REQUEST_URI:
        return "HTTP_REQUEST_URI";
    case HTTP_REQUEST_URI_SP:
        return "HTTP_REQUEST_URI_SP";
    case HTTP_VERSION:
        return "HTTP_VERSION";
    case HTTP_VERSION_CR:
        return "HTTP_VERSION_CR";
    case HTTP_HEADER_START:
        return "HTTP_HEADER_START";
    case HTTP_HEADER_KEY:
        return "HTTP_HEADER_KEY";
    case HTTP_HEADER_SP:
        return "HTTP_HEADER_SP";
    case HTTP_HEADER_VAL:
        return "HTTP_HEADER_VAL";
    case HTTP_HEADER_NL:
        return "HTTP_HEADER_NL";
    case HTTP_BODY_START:
        return "HTTP_BODY_START";
    case HTTP_BODY:
        return "HTTP_BODY";
    default:
        return "UNKNOWN_STATE";
    }
}

// Utility to decode chunked transfer encoding
std::string decode_chunked_body(const std::string& raw_body) {
    std::cout << "[DEBUG] Raw chunked body received (size=" << raw_body.size() << "):\n" << raw_body << std::endl;
    std::string decoded;
    size_t pos = 0;
    while (pos < raw_body.size()) {
        // Find the next CRLF for the chunk size
        size_t crlf = raw_body.find("\r\n", pos);
        if (crlf == std::string::npos) break;
        std::string size_str = raw_body.substr(pos, crlf - pos);
        int chunk_size = strtol(size_str.c_str(), NULL, 16);
        if (chunk_size == 0) break;
        pos = crlf + 2;
        if (pos + chunk_size > raw_body.size()) break;
        decoded.append(raw_body.substr(pos, chunk_size));
        pos += chunk_size + 2; // skip chunk and trailing CRLF
    }
    std::cout << "[DEBUG] Decoded chunked body (size=" << decoded.size() << "):\n" << decoded << std::endl;
    return decoded;
}