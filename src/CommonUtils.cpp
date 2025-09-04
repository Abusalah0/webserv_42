#include "../include/CommonUtils.hpp"
#include "../include/Exceptions.hpp"
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <ctime>
#include <iostream>

/* 			Media Type Task :) 	     	*/

static std::map<std::string, const char *> init_media_types()
// init all the media types.
{
    std::map<std::string, const char*> media_types;
    media_types[".aac"] = "audio/aac";
    media_types[".abw"] = "application/x-abiword";
    media_types[".apng"] = "image/apng";
    media_types[".arc"] = "application/x-freearc";
    media_types[".avif"] = "image/avif";
    media_types[".avi"] = "video/x-msvideo";
    media_types[".azw"] = "application/vnd.amazon.ebook";
    media_types[".bin"] = "application/octet-stream";
    media_types[".bmp"] = "image/bmp";
    media_types[".bz"] = "application/x-bzip";
    media_types[".bz2"] = "application/x-bzip2";
    media_types[".cda"] = "application/x-cdf";
    media_types[".csh"] = "application/x-csh";
    media_types[".css"] = "text/css";
    media_types[".csv"] = "text/csv";
    media_types[".doc"] = "application/msword";
    media_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    media_types[".eot"] = "application/vnd.ms-fontobject";
    media_types[".epub"] = "application/epub+zip";
    media_types[".gz"] = "application/gzip";
    media_types[".gif"] = "image/gif";
    media_types[".htm"] = "text/html";
    media_types[".html"] = "text/html";
    media_types[".ico"] = "image/vnd.microsoft.icon";
    media_types[".ics"] = "text/calendar";
    media_types[".jar"] = "application/java-archive";
    media_types[".jpeg"] = "image/jpeg";
    media_types[".jpg"] = "image/jpeg";
    media_types[".js"] = "text/javascript";
    media_types[".json"] = "application/json";
    media_types[".jsonld"] = "application/ld+json";
    media_types[".md"] = "text/markdown";
    media_types[".mid"] = "audio/midi";
    media_types[".midi"] = "audio/midi";
    media_types[".mjs"] = "text/javascript";
    media_types[".mp3"] = "audio/mpeg";
    media_types[".mp4"] = "video/mp4";
    media_types[".mpeg"] = "video/mpeg";
    media_types[".mpkg"] = "application/vnd.apple.installer+xml";
    media_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
    media_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    media_types[".odt"] = "application/vnd.oasis.opendocument.text";
    media_types[".oga"] = "audio/ogg";
    media_types[".ogv"] = "video/ogg";
    media_types[".ogx"] = "application/ogg";
    media_types[".opus"] = "audio/ogg";
    media_types[".otf"] = "font/otf";
    media_types[".png"] = "image/png";
    media_types[".pdf"] = "application/pdf";
    media_types[".php"] = "application/x-httpd-php";
    media_types[".ppt"] = "application/vnd.ms-powerpoint";
    media_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    media_types[".rar"] = "application/vnd.rar";
    media_types[".rtf"] = "application/rtf";
    media_types[".sh"] = "application/x-sh";
    media_types[".svg"] = "image/svg+xml";
    media_types[".tar"] = "application/x-tar";
    media_types[".tif"] = "image/tiff";
    media_types[".tiff"] = "image/tiff";
    media_types[".ts"] = "video/mp2t";
    media_types[".ttf"] = "font/ttf";
    media_types[".txt"] = "text/plain";
    media_types[".vsd"] = "application/vnd.visio";
    media_types[".wav"] = "audio/wav";
    media_types[".weba"] = "audio/webm";
    media_types[".webm"] = "video/webm";
    media_types[".webmanifest"] = "application/manifest+json";
    media_types[".webp"] = "image/webp";
    media_types[".woff"] = "font/woff";
    media_types[".woff2"] = "font/woff2";
    media_types[".xhtml"] = "application/xhtml+xml";
    media_types[".xls"] = "application/vnd.ms-excel";
    media_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    media_types[".xml"] = "application/xml";
    media_types[".xul"] = "application/vnd.mozilla.xul+xml";
    media_types[".zip"] = "application/zip";
    media_types[".3gp"] = "video/3gpp";
    media_types[".3g2"] = "video/3gpp2";
    media_types[".7z"] = "application/x-7z-compressed";
    return (media_types);
}

const char *get_media_type(const std::string &file_path)
{
    static const std::map<std::string, const char*> media_types = init_media_types();
    size_t dot_pos = file_path.find('.');
    if (dot_pos == std::string::npos)
    {
        return ("application/octet-stream"); // can not find (.) in the path.
    }
    std::string extention = file_path.substr(dot_pos);
    std::transform(extention.begin(), extention.end(), extention.begin(), tolower); // change string to lowercase.
    std::map<std::string, const char *>::const_iterator it = media_types.find(extention); // iterator to find if the extention exist or not.
    if (it != media_types.end())
        return (it->second);
    return ("application/octet-stream");
}

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
	errno = 0;
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

size_t parse_chunk_size(std::string& str)
{
	if (str.size() > 14)
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!std::isxdigit(str[i]))
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}
	size_t chunk_size = strtoul(str.c_str(), 0, 16);
	return chunk_size;
}

std::string ul_to_str(size_t value)
{
	std::string res;

	if (!value)
	{
		res.push_back('0');
		return res;
	}
	while (value)
	{
		res.push_back(value % 10 + 0x30);
		value /= 10;
	}
	std::reverse(res.begin(), res.end());
	return res;
}

std::string tmvalue_to_str(int value)
{
	std::string res;

	if (!value)
	{
		res.append("00");
		return res;
	}
	while (value)
	{
		res.push_back(value % 10 + 0x30);
		value /= 10;
	}
	std::reverse(res.begin(), res.end());
	if (res.size() < 2)
		res.insert(res.begin(), '0');
	return res;
}

void handle_http_file_errno()
{
	switch (errno)
	{
		case EISDIR:
		case EACCES:
			throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
		case ENOENT:
		case ENOTDIR:
			throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
		default:
			throw WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	}
}

std::string concat_path(const std::string& root, const std::string& target)
{
	std::string path = root;
	if (str_back(path) == '/' && !path.empty())
		path.erase(path.size() - 1);
	path.append(target);
	return path;
}

bool is_http_target_file(const std::string& path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
		return false;
	if (S_ISREG(statbuf.st_mode))
		return true;
	return false;
}

bool is_http_target_dir(const std::string& path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
		return false;
	if (S_ISDIR(statbuf.st_mode))
		return true;
	return false;
}

std::string generate_http_date()
{
	std::string date;
	time_t raw_time = time(0);
	tm* datetime = gmtime(&raw_time);
	date.append(daysArr[datetime->tm_wday]);
	date.append(", ");
	date.append(tmvalue_to_str(datetime->tm_mday));
	date.push_back(' ');
	date.append(monthsArr[datetime->tm_mon]);
	date.push_back(' ');
	date.append(ul_to_str(datetime->tm_year + 1900));
	date.push_back(' ');
	date.append(tmvalue_to_str(datetime->tm_hour));
	date.push_back(':');
	date.append(tmvalue_to_str(datetime->tm_min));
	date.push_back(':');
	date.append(tmvalue_to_str(datetime->tm_sec));
	date.append(" GMT");
	return date;
}

std::string generate_autoindex_date()
{
	std::string date;
	time_t raw_time = time(0);
	tm* datetime = gmtime(&raw_time);
	date.append(tmvalue_to_str(datetime->tm_mday));
	date.push_back('-');
	date.append(monthsArr[datetime->tm_mon]);
	date.push_back('-');
	date.append(tmvalue_to_str(datetime->tm_year + 1900));
	date.push_back(' ');
	date.append(tmvalue_to_str(datetime->tm_hour));
	date.push_back(':');
	date.append(tmvalue_to_str(datetime->tm_min));
	return date;
}

std::deque<AutoIndexEntry> generate_autoindex_entries(const std::string& root, const std::string target)
{
	std::string path = concat_path(root, target);
	DIR* dir_s = opendir(path.c_str());
	if (!dir_s)
		handle_http_file_errno();
	std::deque<AutoIndexEntry> entries;
	dirent* dir_ent = readdir(dir_s);
	std::string ent_name;
	std::string ent_path;
	while (dir_ent)
	{
		ent_name = dir_ent->d_name;
		if (ent_name.compare("."))
		{
			struct stat statbuf;
			ent_path = path + ent_name;
			if (!stat(ent_path.c_str(), &statbuf))
			{
				if (S_ISDIR(statbuf.st_mode))
					ent_name.push_back('/');
				AutoIndexEntry entry;
				entry.ent_name = ent_name;
				entry.statbuf = statbuf;
				entries.push_back(entry);
			}
		}
		dir_ent = readdir(dir_s);
	}
	closedir(dir_s);
	return entries;
}

void replace_template_str(std::string& body,
	const std::string& str_template,
	const std::string& str)
{
	size_t pos = body.find(str_template);
	while (pos != std::string::npos)
	{
		body.erase(pos, str_template.size());
		body.insert(pos, str);
		pos = body.find(str_template, pos + str.size());
	}
}