/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommonUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 00:02:50 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/18 03:01:26 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMONUTILS_HPP
# define COMMONUTILS_HPP

# include <string>
# include <deque>
# include <stdint.h>
# include <cstdlib>
# include <errno.h>
# include <csignal>
# include <vector>
# include <algorithm>
# include <sys/stat.h>
# include <netdb.h>
# include <arpa/inet.h>

extern int g_signum;

typedef enum EConnectionTypes
{
	CONNECTION_KEEP_ALIVE,
	CONNECTION_CLOSE
} ConnectionTypes;

# define SERVER_SOFTWARE "webserv/1.0"

// HTTP Status Codes
# define HTTP_OK 200
# define HTTP_CREATED 201
# define HTTP_NO_CONTENT 204
# define HTTP_MOVED_PERMANENTLY 301
# define HTTP_FOUND 302
# define HTTP_BAD_REQUEST 400
# define HTTP_FORBIDDEN 403
# define HTTP_NOT_FOUND 404
# define HTTP_METHOD_NOT_ALLOWED 405
# define HTTP_CONTENT_TOO_LARGE 413
# define HTTP_IAM_A_TEAPOT 418
# define HTTP_INTERNAL_SERVER_ERROR 500
# define HTTP_NOT_IMPLEMENTED 501
# define HTTP_BAD_GATEWAY 502
# define HTTP_GATEWAY_TIMEOUT 504
# define HTTP_VERSION_ERROR 505

// HTTP Status Messages
# define HTTP_OK_MSG "200 OK"
# define HTTP_CREATED_MSG "201 Created"
# define HTTP_NO_CONTENT_MSG "204 No Content"
# define HTTP_MOVED_PERMANENTLY_MSG "301 Moved Permanetly"
# define HTTP_FOUND_MSG "302 Found"
# define HTTP_BAD_REQUEST_MSG "400 Bad Request"
# define HTTP_FORBIDDEN_MSG "403 Forbidden"
# define HTTP_NOT_FOUND_MSG "404 Not Found"
# define HTTP_METHOD_NOT_ALLOWED_MSG "405 Method Not Allowed"
# define HTTP_CONTENT_TOO_LARGE_MSG "413 Content Too Large"
# define HTTP_IAM_A_TEAPOT_MSG "418 I'm a teapot"
# define HTTP_INTERNAL_SERVER_ERROR_MSG "500 Internal Server Error"
# define HTTP_NOT_IMPLEMENTED_MSG "501 Not Implemented"
# define HTTP_BAD_GATEWAY_MSG "502 Bad Gateway"
# define HTTP_GATEWAY_TIMEOUT_MSG "504 Gateway TImeout"
# define HTTP_VERSION_ERROR_MSG "505 HTTP Version Not Supported"

// Used when path of root is not absoulte or relative
# define ROOT_PREFIX "/var/lib/webserv/"
// default root path
# define DEFAULT_ROOT_PATH "/var/lib/webserv/html"

// Units of measure
# define KILOBYTE 1024
# define MEGABYTE 1048576
# define GIGABYTE 1073741824
// Units of measure limits
# define MAX_KILOBYTE 18014398509481984UL
# define MAX_MEGABYTE 17592186044416UL
# define MAX_GIGABYTE 17179869184UL

# define WEBSERV_INTERNAL_REDIRECT_LIMIT 10

# define CHUNK_SIZE KILOBYTE * 8

// AutoIndexEntry structure for directory listings
struct AutoIndexEntry
{
	std::string ent_name;
	struct stat statbuf;
};

static const std::string daysArr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const std::string monthsArr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char hex_base[17] = "0123456789abcdef";

/**
 * Getter for last character reference in string
 * @param str string reference
 * @return last character reference or null reference
 */
const char& str_back(const std::string& str);

/**
 * Parse HTTP code with validation
 * @param str HTTP code as string
 * @return HTTP code as ushort
 */
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

/**
 * @brief Get the media type for a given file path.
 * @param file_path The file path to check.
 * @return The media type as a const char pointer.
 */
const char *get_media_type(const std::string &file_path);

/**
 * @brief Handle file-related errno and throw appropriate HTTP exceptions.
 * @throw WebservExceptions::HTTPException with corresponding HTTP status code.
 */
void handle_http_file_errno();

/**
 * @brief Concatenate root and target paths.
 * @param root The root path.
 * @param target The target path.
 * @return The concatenated path.
 */
std::string concat_path(const std::string& root, const std::string& target);

/**
 * @brief Check if the given path is a regular file.
 * @param path The file path to check.
 * @return true if the path is a regular file, false otherwise.
 */
bool is_http_target_file(const std::string& path);

/**
 * @brief Check if the given path is a directory.
 * @param path The directory path to check.
 * @return true if the path is a directory, false otherwise.
 */
bool is_http_target_dir(const std::string& path);

/**
 * @brief Generate the current HTTP date string.
 * @return The generated HTTP date string.
 */
std::string generate_http_date();

/**
 * @brief Generate the HTTP date string for a given time.
 * @param raw_time The time value to generate the date string for.
 * @return The generated HTTP date string.
 */
std::string generate_http_date(time_t raw_time);

/**
 * @brief Generate the current autoindex date string.
 * @return The generated autoindex date string.
 */
std::string generate_autoindex_date();

/**
 * @brief Generate autoindex entries for a given root and target.
 * @param root The root directory.
 * @param target The target directory.
 * @return A deque containing the generated autoindex entries.
 */
std::deque<AutoIndexEntry> generate_autoindex_entries(const std::string& root, const std::string target);

/**
 * @brief Replace a template string in the body with the provided string.
 * @param body The body string to modify.
 * @param str_template The template string to replace.
 * @param str The string to replace the template with.
 */
void replace_template_str(std::string& body,
	const std::string& str_template,
	const std::string& str);

/**
 * @brief Parse a sockaddr_in structure to extract the IP address and port.
 * @param sockaddr The sockaddr_in structure to parse.
 * @return A pair containing the IP address as a string and the port as a string.
 * @note The returned pair's first element is the IP address, and the second element is the port.
 */
std::pair<std::string, std::string> parse_sockaddr(sockaddr_in& sockaddr);

/**
 * @brief Check if a CGI field name is valid.
 * @param name The CGI field name to check.
 * @return true if the field name is valid, false otherwise.
 */
bool is_field_cgi_valid(const std::string& name);

/**
 * @brief Convert a character to its CGI equivalent.
 * @param c The character to convert.
 * @return The converted CGI character.
 */
char chr_to_cgi(char c);

/**
 * @brief Convert an unsigned long value to a hexadecimal string.
 * @param value The unsigned long value to convert.
 * @return The converted hexadecimal string.
 */
std::string ul_to_hex(size_t value);

/**
 * @brief Check if a character is a digit (0-9).
 * @param c The character to check.
 * @return true if the character is a digit, false otherwise.
 */
bool c_isdigit(u_char c);

/**
 * @brief Validate if the response status code in a field value is valid.
 * @param field_value The field value containing the response status code.
 * @return true if the response status code is valid, false otherwise.
 */
bool is_response_status_valid(const std::string& field_value);

/**
 * @brief Extracts the components of a route from a given string, splits by '/'.
 * @param str The input string representing the route.
 * @return A deque containing the components of the route. 
 */
std::deque<std::string> extract_route_components(const std::string& str);

#endif