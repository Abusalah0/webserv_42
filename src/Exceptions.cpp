#include "../include/Exceptions.hpp"

const char* WebservExceptions::FileOpenFailure::what() const throw()
{
    return "Opening file failed!";
}

const char* WebservExceptions::ADefaultServerAlreadyExists::what() const throw()
{
    return "A default server already exists!";
}