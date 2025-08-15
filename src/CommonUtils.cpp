#include "../include/CommonUtils.hpp"

/**
 * Getter for last character reference in string
 * @param str string reference
 * @return last character reference or null reference
 */
const char& str_back(const std::string& str)
{
	static const char null_chr = '\0';
    if (str.empty())
        return null_chr;
    return str[str.size() - 1];
}

/**
 * Parse HTTP code with validation
 * @param str HTTP code as string
 * @return HTTP code as ushort
 */
ushort parse_http_code(const std::string& str)
{
	char* endptr;

	if (str.empty())
		throw WebservExceptions::InvalidValue();
	if (!std::isdigit(str[0]))
		throw WebservExceptions::InvalidValue();
	long code = strtol(str.c_str(), &endptr, 10);
	if (*endptr || errno == ERANGE)
		throw WebservExceptions::InvalidValue();
	if (code < 0 || code > 999)
		throw WebservExceptions::HttpCodeOutOfRange();
	return code;
}