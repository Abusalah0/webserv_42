#include "../include/CommonUtils.hpp"

const char& str_back(const std::string& str)
{
	static const char null_chr = '\0';
    if (str.empty())
        return null_chr;
    return str[str.size() - 1];
}