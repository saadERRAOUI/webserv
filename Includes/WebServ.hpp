/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serraoui <serraoui@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:58:33 by serraoui          #+#    #+#             */
/*   Updated: 2024/12/23 16:12:28 by serraoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//Include std libraries
//Add Macros
//Add Constants

#include <iostream>
#include <string>
#include <algorithm>

enum HttpRequestState {
    HTTP_METHOD,
    HTTP_REQUEST_URI,
    HTTP_VERSION,
    HTTP_HEADERS,
    HTTP_BODY,
    HTTP_COMPLETE,
    HTTP_ERROR
};



#endif WEBSERV_HPP