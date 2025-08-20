#include "../include/Exceptions.hpp"

const char* WebservExceptions::FileOpenFailure::what() const throw()
{
    return "Opening file failed.";
}

const char* WebservExceptions::InvalidValue::what() const throw()
{
    return "Invalid value.";
}

const char* WebservExceptions::HttpCodeOutOfRange::what() const throw()
{
    return "HTTP code out of range.";
}

const char* WebservExceptions::HttpRedirectCodeOutOfRange::what() const throw()
{
    return "HTTP redirect code out of range.";
}

const char* WebservExceptions::HttpErrorCodeOutOfRange::what() const throw()
{
    return "HTTP error code out of range.";
}

const char* WebservExceptions::NoAvailablePage::what() const throw()
{
    return "No available page.";
}

const char* WebservExceptions::ForbiddenAccess::what() const throw()
{
    return "Forbidden access.";
}

const char* WebservExceptions::NonRegularFile::what() const throw()
{
    return "Non regular file.";
}

const char* WebservExceptions::InternalRedirectsLimitReached::what() const throw()
{
    return "Internal redirects limit reached.";
}

const char* WebservExceptions::ADefaultServerAlreadyExists::what() const throw()
{
    return "A default server already exists!";
}

const char* WebservExceptions::SocketFailed::what() const throw()
{
    return "Socket failed!";
}

const char* WebservExceptions::GAIFailed::what() const throw()
{
    return "getaddrinfo failed!";
}

const char* WebservExceptions::BindFailed::what() const throw()
{
    return "Bind failed!";
}

const char* WebservExceptions::ListenFailed::what() const throw()
{
    return "Listen failed!";
}