/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:45:35 by serraoui          #+#    #+#             */
/*   Updated: 2025/05/10 13:56:33 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include "HttpRequest.hpp"
#include <string>
#include <cctype>

enum ParseResult {
    PARSE_START,
    PARSE_INPROGRESS,
    PARSE_SUCCESS,
    PARSE_ERROR
};

class HttpRequestParser {
    private :
        /*
            Parsers map handlers
        */
        
        std::map<HttpRequestState, ParseResult (HttpRequestParser::*)(HttpRequest&, char)> _methodHandlerMap;


        ParseResult parseHttpMethod(HttpRequest& request, char byte);
        ParseResult parseHttpRequestUriMethod(HttpRequest& request, char byte);
        ParseResult parseHttpVersionMethod(HttpRequest& request, char byte);
        ParseResult parseHttpHeadersMethod(HttpRequest& request, char byte);
        ParseResult parseHttpBodyMethod(HttpRequest& request, char byte);

    public :
        /*
            Constructor & Destructor
        */
        HttpRequestParser();
        ~HttpRequestParser();

        /*
            Utility methods
        */
        bool	isChar(char) const;
		bool	isControl(char) const;
		bool	isSpecial(char) const;
		bool	isDigit(char) const;
        
        std::string getStateName(HttpRequestState state) const;
        /*
            Parser Member methods
        */
        ParseResult            parse(HttpRequest&, char);
};