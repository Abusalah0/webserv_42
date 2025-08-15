#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>
#include <stdint.h>
#include <cstdlib>
#include <errno.h>
#include "Exceptions.hpp"

// Used when path of root is not absoulte or relative
#define ROOT_PREFIX "/var/lib/webserv/"
// default root path
#define DEFAULT_ROOT_PATH "/var/lib/webserv/html"

// Units of measure
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824
// Units of measure limits
#define MAX_KILOBYTE 18014398509481984UL
#define MAX_MEGABYTE 17592186044416UL
#define MAX_GIGABYTE 17179869184UL

#define WEBSERV_INTERNAL_REDIRECT_LIMIT 10

const char& str_back(const std::string& str);
ushort parse_http_code(const std::string& str);

#endif