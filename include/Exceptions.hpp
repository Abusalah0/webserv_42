/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:53 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/14 17:16:18 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

namespace WebservExceptions
{
    class FileOpenFailure: public std::exception
    {
        const char* what() const throw();
    };
    class InvalidValue: public std::exception
    {
        const char* what() const throw();
    };
	class HttpCodeOutOfRange: public std::exception
    {
        const char* what() const throw();
    };
	class HttpRedirectCodeOutOfRange: public std::exception
    {
        const char* what() const throw();
    };
	class HttpErrorCodeOutOfRange: public std::exception
    {
        const char* what() const throw();
    };
	class NoAvailablePage: public std::exception
	{
		const char* what() const throw();
	};
	class ForbiddenAccess: public std::exception
	{
		const char* what() const throw();
	};
	class NonRegularFile: public std::exception
	{
		const char* what() const throw();
	};
	class InternalRedirectsLimitReached: public std::exception
	{
		const char* what() const throw();
	};
    class ADefaultServerAlreadyExists: public std::exception
    {
        const char* what() const throw();
    };
    class SocketFailed: public std::exception
    {
        const char* what() const throw();
    };
    class GAIFailed: public std::exception
    {
        const char* what() const throw();
    };
    class BindFailed: public std::exception
    {
        const char* what() const throw();
    };
    class ListenFailed: public std::exception
    {
        const char* what() const throw();
    };
}

#endif
