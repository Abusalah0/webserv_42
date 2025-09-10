#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>
#include <deque>
#include <stdint.h>
#include <cstdlib>
#include <errno.h>
#include <csignal>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>

extern int g_signum;

typedef enum EConnectionTypes
{
	CONNECTION_KEEP_ALIVE,
	CONNECTION_CLOSE
} ConnectionTypes;

#define SERVER_SOFTWARE "webserv/1.0"

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_NO_CONTENT 204
#define HTTP_MOVED_PERMANENTLY 301
#define HTTP_FOUND 302
#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_CONTENT_TOO_LARGE 413
#define HTTP_INTERNAL_SERVER_ERROR 500
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_GATEWAY_TIMEOUT 504
#define HTTP_VERSION_ERROR 505

#define HTTP_OK_MSG "200 OK"
#define HTTP_CREATED_MSG "201 Created"
#define HTTP_NO_CONTENT_MSG "204 No Content"
#define HTTP_MOVED_PERMANENTLY_MSG "301 Moved Permanetly"
#define HTTP_FOUND_MSG "302 Found"
#define HTTP_BAD_REQUEST_MSG "400 Bad Request"
#define HTTP_FORBIDDEN_MSG "403 Forbidden"
#define HTTP_NOT_FOUND_MSG "404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED_MSG "405 Method Not Allowed"
#define HTTP_CONTENT_TOO_LARGE_MSG "413 Content Too Large"
#define HTTP_INTERNAL_SERVER_ERROR_MSG "500 Internal Server Error"
#define HTTP_NOT_IMPLEMENTED_MSG "501 Not Implemented"
#define HTTP_BAD_GATEWAY_MSG "502 Bad Gateway"
#define HTTP_GATEWAY_TIMEOUT_MSG "504 Gateway TImeout"
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

struct AutoIndexEntry
{
	std::string ent_name;
	struct stat statbuf;
};

static const std::string daysArr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const std::string monthsArr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char hex_base[17] = "0123456789abcdef";

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
void handle_http_file_errno();
std::string concat_path(const std::string& root, const std::string& target);
bool is_http_target_file(const std::string& path);
bool is_http_target_dir(const std::string& path);
std::string generate_http_date();
std::string generate_http_date(time_t raw_time);
std::string generate_autoindex_date();
std::deque<AutoIndexEntry> generate_autoindex_entries(const std::string& root, const std::string target);
void replace_template_str(std::string& body,
	const std::string& str_template,
	const std::string& str);
std::pair<std::string, std::string> parse_sockaddr(sockaddr_in& sockaddr);
bool is_field_cgi_valid(const std::string& name);
char chr_to_cgi(char c);
std::string ul_to_hex(size_t value);
bool c_isdigit(u_char c);
bool is_response_status_valid(const std::string& field_value);

#endif