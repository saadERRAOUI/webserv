/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:00 by serraoui          #+#    #+#             */
/*   Updated: 2025/02/23 17:27:06 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include "../Includes/WebServ.hpp"

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

        /*
            Parser Member methods
        */
        void            parseHttpMethod(char);
        void            parseHttpRequestUriMethod(char);
        void            parseHttpVersionMethod(char);
        void            parseHttpHeadersMethod(char);
        void            parseHttpBodyMethod(char);
        
        void            parseHttpRequestOrchestrator(char);
};

#endif
