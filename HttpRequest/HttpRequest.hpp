/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hitchman <hitchman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:00 by serraoui          #+#    #+#             */
/*   Updated: 2025/04/16 20:06:04 by hitchman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include "../Includes/Webserv.hpp"

enum HttpRequestState {
    HTTP_METHOD,
    HTTP_REQUEST_URI,
    HTTP_VERSION,
    HTTP_HEADERS,
    HTTP_BODY
};

class HttpRequest {
    private :
        std::string                         _method;
        std::string                         _requestURI;
        std::string                         _version;
        std::string                         _body;
        std::map<std::string, std::string>  _headers;
        int                                 _state;
        /*
            Parsers map handlers
        */
        std::map<HttpRequestState, void (HttpRequest::*)(char)> _methodHandlerMap;
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

        /*
            Setters
        */
        void            setMethod(std::string);
        void            setRequestURI(std::string);
        void            setVersion(std::string);
        void            setBodyFd(int);
        void            setHeaders(std::string key, std::string value);
        /*
            Parser Member methods
        */
        void            parseHttpMethod(char);
        void            parseHttpRequestUriMethod(char);
        void            parseHttpVersionMethod(char);
        void            parseHttpHeadersMethod(char);
        void            parseHttpBodyMethod(char);

        void            parseHttpRequestOrchestrator(char);

        // added by hicham 2025-05-05 , used for response.
        void            ClearURI();
};

#endif
