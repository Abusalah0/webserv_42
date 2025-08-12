#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>

// Used when path of root is not absoulte or relative
#define ROOT_PREFIX "/var/lib/pginx/"
// default root path
#define DEFAULT_ROOT_PATH "/var/lib/pginx/html/"

// Units of measure
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824
// Units of measure limits
#define MAX_KILOBYTE 18014398509481984UL
#define MAX_MEGABYTE 17592186044416UL
#define MAX_GIGABYTE 17179869184UL

const char& str_back(const std::string& str);

#endif