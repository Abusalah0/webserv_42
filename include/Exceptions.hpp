/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:53 by sbibers           #+#    #+#             */
/*   Updated: 2025/09/03 22:16:03 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include "CommonUtils.hpp"

struct HTTPStatus
{
	ushort code;
	const char* msg;
};

static const HTTPStatus statusTable[] = {
	{HTTP_OK, HTTP_OK_MSG},
    {HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG},
	{HTTP_FOUND, HTTP_FOUND_MSG},
	{HTTP_BAD_REQUEST, HTTP_BAD_REQUEST_MSG},
	{HTTP_FORBIDDEN, HTTP_FORBIDDEN_MSG},
	{HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG},
	{HTTP_METHOD_NOT_ALLOWED, HTTP_METHOD_NOT_ALLOWED_MSG},
	{HTTP_CONTENT_TOO_LARGE, HTTP_CONTENT_TOO_LARGE_MSG},
	{HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG},
	{HTTP_NOT_IMPLEMENTED, HTTP_NOT_IMPLEMENTED_MSG},
	{HTTP_BAD_GATEWAY, HTTP_BAD_GATEWAY_MSG},
    {HTTP_VERSION_ERROR, HTTP_VERSION_ERROR_MSG},
	{0, 0}
};

namespace WebservExceptions
{
	class HTTPException: public std::exception
	{
		private:
			ushort m_code;
			const char* m_msg;
		public:
			HTTPException(ushort code);
			virtual ~HTTPException() throw();
			ushort get_error_code() const;
			const char* what() const throw();
	};
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
	class PollFailed: public std::exception
    {
        const char* what() const throw();
    };
	class SetupFailed: public std::exception
    {
        const char* what() const throw();
    };
	class MultipleDefaultServers: public std::exception
    {
        const char* what() const throw();
    };
	class DirectServFailed: public std::exception
    {
        const char* what() const throw();
    };
	class LocationNotFound: public std::exception
    {
        const char* what() const throw();
    };
}

#endif
