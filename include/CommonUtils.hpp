#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>
#include <stdint.h>
#include <cstdlib>
#include <errno.h>
#include <csignal>
#include <vector>
#include <algorithm>
#include "Exceptions.hpp"

extern int g_signum;

typedef enum EConnectionTypes
{
	CONNECTION_KEEP_ALIVE,
	CONNECTION_CLOSE
} ConnectionTypes;

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

#define CHUNK_SIZE KILOBYTE * 8

const char& str_back(const std::string& str);
ushort parse_http_code(const std::string& str);
/**
* Checks if character is valid in token
* @param c character
* @return If character is valid in token or not
*/
bool is_token_chr(u_char c);
/**
* Checks if character is valid whitespace
* @param c character
* @return If character is valid whitespace or not
*/
bool is_ws_chr(u_char c);
/**
* Checks if character is vchar (Visible Character)
* @param c character
* @return If character is vchar or not
*/
bool is_vchar(u_char c);
/**
* Checks if character is obs-text (Obsolete character for backward compability)
* @param c character
* @return If character is obs-text or not
*/
bool is_obs_chr(u_char c);
/**
* Checks if character is valid field-value
* @param c character
* @return If character is valid field-value
*/
bool is_field_value_chr(u_char c);
/**
* Function to run on string to check if
* each character in the string followes the rules of provided function
* @param str String
* @param func Function to check each character
* @return If all characters followes the function rule or not
*/
bool check_str_chrs(std::string& str, bool (*func)(u_char c));
size_t parse_chunk_size(std::string& str);
std::string normalize_path(const std::string& path);
std::string url_decode(const std::string& encoded);
std::string ul_to_str(size_t value);

#endif