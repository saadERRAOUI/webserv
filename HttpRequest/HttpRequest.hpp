/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:00 by serraoui          #+#    #+#             */       
/*   Updated: 2025/04/27 19:54:03 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <map>
#include "../Includes/Webserv.hpp"

#define BUFFER_SIZE 8000
/* Add State enum */
enum HttpRequestState {
    /* old */
    HTTP_METHOD_START,
    HTTP_METHOD,
    HTTP_METHOD_SP,
    HTTP_REQUEST_URI,
    HTTP_QUERY_STRING_START,
	HTTP_QUERY_STRING,
	HTTP_FRAGMENT_START,
	HTTP_FRAGMENT,
    HTTP_REQUEST_URI_SP,
    HTTP_VERSION,
    HTTP_VERSION_CR,
    HTTP_HEADER_START,
    HTTP_HEADER_KEY,
    HTTP_HEADER_SP,
    HTTP_HEADER_VAL,
    HTTP_HEADER_CR,
    HTTP_HEADER_NL,
    HTTP_BODY_START,
    HTTP_BODY
};

class HttpRequest {
    private :
        std::string                         _method; //Get, Post, Delete
        std::string                         _requestURI;// "/users - *" -> /users/temp 
        std::string                         _version; //"1.1, 1.0"
        std::string                         _body; //"content"
        std::map<std::string, std::string>  _headers; // <Host, example.com>
        std::map<std::string, std::string>  _cookies;
        
        /* utility variables */
        int                                 _state;
        bool                                isCGI;
        /*
            Parsers map handlers
        */
        std::map<HttpRequestState, void (HttpRequest::*)(char)> _methodHandlerMap;
        bool                                _isChunked;
        // int                                 _bodyFd;
        std::string                         _headerKey;
        std::string                         _headerValue;
        int                                 _endSequenceState;
        std::string                         _queryString;
        std::string                         _fragment;

    public :
        /*
            Constructors & Destructors
        */
        HttpRequest();
        ~HttpRequest();
        HttpRequest &operator=(const HttpRequest &copy_HttpRequest);


        /*
            Getters
        */
        std::string                         getMethod() const;
        std::string                         getRequestURI() const;
        std::string                         getVersion() const;
        std::string                         getBody() const;
        std::map<std::string, std::string>  getHeaders() const;
        int                                 getState() const;
        std::map<std::string, std::string>  getQueryParams() const;
        std::string                         getHeader(std::string key) const;
        bool                                getIsCGI();
        std::string                         getHeaderKey() const;
        std::string                         getHeaderValue() const;
        int                                 getEndSequenceState() const;
        bool                                getIsChunked() const;
        std::string                         getQueryString() const;
        std::string                         getFragment() const;
        /*
            Setters
        */
        void            setMethod(std::string);
        void            setIsCGI(bool iscgi);
        void            setRequestURI(std::string);
        void            setVersion(std::string);
        void            setBodyFd(int);

        // added by hicham 2025-05-05 , used for response.
        void            ClearURI();
        void            setHeaders(std::string, std::string);
        void            setHeaderKey(std::string);
        void            setHeaderValue(std::string);
        void            setEndSequenceState(int);
        void            setBody(std::string);
        void            setState(int);
        void            setIsChunked(bool);
        void            setQueryString(std::string);
        void            setFragment(std::string);

        void            showRequest() const;

        void parseCookies();
        std::string getCookie(const std::string& name) const;
};
