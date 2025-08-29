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

bool is_token_chr(u_char c)
{
	if (c != '!' && c != '#' && c != '$'
		&& c != '%' && c != '&' && c != '\''
		&& c != '*' && c != '+' && c != '-'
		&& c != '.' && c != '^' && c != '_'
		&& c != '`' && c != '|' && c != '~'
		&& !std::isalnum(c))
		return false;
	return true;
}

bool is_ws_chr(u_char c)
{
	if (c == ' ' || c == '\t')
		return true;
	return false;
}

bool is_vchar(u_char c)
{
	if (c >= 0x21 && c <= 0x7E)
		return true;
	return false;
}

bool is_obs_chr(u_char c)
{
	if (c >= 0x80)
		return true;
	return false;
}

bool is_field_value_chr(u_char c)
{
	if (!is_ws_chr(c) && !is_vchar(c) && !is_obs_chr(c))
		return false;
	return true;
}

bool check_str_chrs(std::string& str, bool (*func)(u_char c))
{
	size_t i = 0;
	while (i < str.size())
	{
		if (!func(str[i]))
			return false;
		i++;
	}
	return true;
}