/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:53 by sbibers           #+#    #+#             */
/*   Updated: 2025/09/01 02:04:02 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include "CommonUtils.hpp"

#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_VERSION_ERROR 505

struct HTTPStatus
{
	ushort code;
	const char* msg;
};

static const HTTPStatus statusTable[] = {
	{HTTP_BAD_REQUEST, "Bad Request"},
	{HTTP_FORBIDDEN, "Forbidden"},
	{HTTP_NOT_FOUND, "Not Found"},
	{HTTP_NOT_IMPLEMENTED, "Not Implemented"},
	{HTTP_VERSION_ERROR, "HTTP Version Not Supported"},
	{HTTP_BAD_GATEWAY, "Bad Gateway"},
	{0, 0}
};

namespace WebservExceptions
{
	class HTTPException: public std::exception
	{
		private:
			ushort m_code;
			std::string m_msg;
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
}

#endif
