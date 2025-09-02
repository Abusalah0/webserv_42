#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>
#include <stdint.h>
#include <cstdlib>
#include <errno.h>
#include <csignal>
#include <vector>
#include <algorithm>

extern int g_signum;

typedef enum EConnectionTypes
{
	CONNECTION_KEEP_ALIVE,
	CONNECTION_CLOSE
} ConnectionTypes;

#define HTTP_OK 200
#define HTTP_MOVED_PERMANENTLY 301
#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_VERSION_ERROR 505

#define HTTP_OK_MSG "200 OK"
#define HTTP_MOVED_PERMANENTLY_MSG "301 Moved Permanetly"
#define HTTP_BAD_REQUEST_MSG "400 Bad Request"
#define HTTP_FORBIDDEN_MSG "403 Forbidden"
#define HTTP_NOT_FOUND_MSG "404 Not Found"
#define HTTP_NOT_IMPLEMENTED_MSG "501 Not Implemented"
#define HTTP_BAD_GATEWAY_MSG "502 Bad Gateway"
#define HTTP_VERSION_ERROR_MSG "505 HTTP Version Not Supported"

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
/**
 * Parses the chunk size from a string.
 * @param str The string to parse.
 * @return The parsed chunk size.
 */
size_t parse_chunk_size(std::string& str);
/**
 * Normalizes a file path by removing redundant components.
 * @param path The file path to normalize.
 * @return The normalized file path.
 */
std::string normalize_path(const std::string& path);
/**
 * Decodes a URL-encoded string.
 * @param encoded The URL-encoded string to decode.
 * @return The decoded string.
 */
std::string url_decode(const std::string& encoded);
/**
 * Converts a size_t value to a string.
 * @param value The size_t value to convert.
 * @return The converted string.
 */
std::string ul_to_str(size_t value);
const char *get_media_type(const std::string &file_path);

#endif