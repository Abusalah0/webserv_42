#include "../include/Exceptions.hpp"

const char* WebservExceptions::FileOpenFailure::what() const throw()
{
    return "Opening file failed!";
}