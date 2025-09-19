#include "../include/Exceptions.hpp"

std::string status_table_lookup(ushort code)
{
    // Lookup the status code in the status table
    for (int i = 0; statusTable[i].code != 0; ++i)
	{
        if (statusTable[i].code == code)
            return statusTable[i].msg;// found the code, return the message
    }
    // If the code is not found, return a default message
    std::string res = ul_to_str(code);
    res.append(" Unknown Error");

    return (res);
}

WebservExceptions::HTTPException::HTTPException(ushort code):
	m_code(code)
{
	this->m_msg = status_table_lookup(code);
}

WebservExceptions::HTTPException::~HTTPException() throw()
{}

ushort WebservExceptions::HTTPException::get_error_code() const
{
	return (this->m_code);
}

const char* WebservExceptions::HTTPException::what() const throw()
{
	return (this->m_msg.c_str());
}

const char* WebservExceptions::FileOpenFailure::what() const throw()
{
    return ("Opening file failed.");
}

const char* WebservExceptions::InvalidValue::what() const throw()
{
    return ("Invalid value.");
}

const char* WebservExceptions::HttpCodeOutOfRange::what() const throw()
{
    return ("HTTP code out of range.");
}

const char* WebservExceptions::HttpRedirectCodeOutOfRange::what() const throw()
{
    return ("HTTP redirect code out of range.");
}

const char* WebservExceptions::HttpErrorCodeOutOfRange::what() const throw()
{
    return ("HTTP error code out of range.");
}

const char* WebservExceptions::NoAvailablePage::what() const throw()
{
    return ("No available page.");
}

const char* WebservExceptions::ForbiddenAccess::what() const throw()
{
    return ("Forbidden access.");
}

const char* WebservExceptions::NonRegularFile::what() const throw()
{
    return ("Non regular file.");
}

const char* WebservExceptions::InternalRedirectsLimitReached::what() const throw()
{
    return ("Internal redirects limit reached.");
}

const char* WebservExceptions::ADefaultServerAlreadyExists::what() const throw()
{
    return ("A default server already exists!");
}

const char* WebservExceptions::SocketFailed::what() const throw()
{
    return ("Socket failed!");
}

const char* WebservExceptions::GAIFailed::what() const throw()
{
    return ("getaddrinfo failed!");
}

const char* WebservExceptions::BindFailed::what() const throw()
{
    return ("Bind failed!");
}

const char* WebservExceptions::ListenFailed::what() const throw()
{
    return ("Listen failed!");
}

const char* WebservExceptions::PollFailed::what() const throw()
{
    return ("Poll failed!");
}

const char* WebservExceptions::SetupFailed::what() const throw()
{
    return ("Webserv setup failed!");
}

const char* WebservExceptions::MultipleDefaultServers::what() const throw()
{
    return ("Multiple default servers exists!");
}

const char* WebservExceptions::DirectServFailed::what() const throw()
{
    return ("Direct serv failed!");
}

const char* WebservExceptions::LocationNotFound::what() const throw()
{
    return ("Location not found!");
}

const char* WebservExceptions::CGINotFound::what() const throw()
{
    return ("CGI not found!");
}

const char* WebservExceptions::ExitChild::what() const throw()
{
    return ("Exit child!");
}

const char* WebservExceptions::PollEntryNotFound::what() const throw()
{
    return ("Poll entry not found!");
}
