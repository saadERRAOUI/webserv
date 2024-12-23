/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:39:00 by serraoui          #+#    #+#             */
/*   Updated: 2024/12/23 15:08:12 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include  "../Includes/WebServ.hpp"

class HttpRequest {
    private :
        //Todo -> Add request params
        //_method
        //_rURI
        //_version
        //_Body -> file (int fd)
        //Map<String, String> headers
        std::string                         _method; 
        std::string                         _requestURI;
        std::string                         _version; 
        int                                 _body;
        std::map<std::string, std::string>  _headers;
    public :
        /*
            Getters
        */
        std::string     getMethod() const;
        std::string     getRequestURI() const;
        std::string     getVersion() const;
        int             getBodyFd() const;
        
        /*
            Setters
        */
        void            setMethod(std::string);
        void            setRequestURI(std::string);
        void            setVersion(std::string);
        void            setBodyFd(int);

        /*
            Member methods
        */
};
