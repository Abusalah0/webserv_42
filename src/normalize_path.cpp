/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalize_path.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 23:33:28 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/31 00:05:20 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Location.hpp"
#include <stdexcept>
#include <cctype>
#include <cstring>

static char char_to_hex(int c)
{
    if (isdigit(c))
        return c - '0';
    else if (isupper(c))
        return c - 'A' + 10;
    else if (islower(c))
        return c - 'a' + 10;
    return 0;
}

static void compress_slashes(std::string& path)
{
    std::string result;
    
    result.reserve(path.length());
    bool prev_slash = false;
    for (size_t i = 0; i < path.length(); ++i)
    {
        if (path[i] == '/')
        {
            if (!prev_slash)
            {
                result += '/';
                prev_slash = true;
            }
        }
        else
        {
            result += path[i];
            prev_slash = false;
        }
    }
    path = result;
}

std::string url_decode(const std::string& encoded)
{
    std::string result;
    result.reserve(encoded.length());
    
    for (size_t i = 0; i < encoded.length(); ++i)
    {
        if (encoded[i] == '%' && i + 2 < encoded.length())
        {
            // Check if next two characters are valid hex digits
            char high = encoded[i + 1];
            char low = encoded[i + 2];

            if ((isdigit(high) || (high >= 'A' && high <= 'F') || (high >= 'a' && high <= 'f')) && 
                (isdigit(low) || (low >= 'A' && low <= 'F') || (low >= 'a' && low <= 'f')))
            {
                int high_val = char_to_hex(high);
                int low_val = char_to_hex(low);
                char decoded_char = static_cast<char>(high_val * 16 + low_val);
                result += decoded_char;
                i += 2; // Skip the next two characters
            }
            else
                result += encoded[i]; // Invalid encoding, keep as is
        }
        else if (encoded[i] == '+')
            result += ' '; // Plus sign decodes to space in form data
        else
            result += encoded[i];
    }
    return (result);
}

static std::vector<std::string> resolve_path_components(const std::string &path)
{
    std::vector<std::string> components;
    std::string component;

    for (size_t i = 1; i < path.length(); ++i) // Skip first '/'
    {
        if (path[i] == '/')
        {
            if (!component.empty())
            {
                if (component == "..")
                {
                    if (!components.empty())
                        components.pop_back();
                }
                else if (component != ".")
                    components.push_back(component);
                component.clear();
            }
        }
        else
            component += path[i];
    }
    // Handle the last component
    if (!component.empty())
    {
        if (component == "..")
        {
            if (!components.empty())
                components.pop_back();
        }
        else if (component != ".")
            components.push_back(component);
    }
    return (components);
}


std::string normalize_path(const std::string& path)
{
    if (path.empty())
        return "/";
    
    std::string result = path;
    // URL decode percent-encoded characters
    result = url_decode(result);
    // Ensure path starts with '/'
    if (result[0] != '/')
        result = "/" + result;
    // Remove duplicate slashes
    compress_slashes(result);    
    // Resolve . and .. components
    std::vector<std::string> components;
    
    components = resolve_path_components(result); 
    // Rebuild the path
    if (components.empty())
        return "/";
    
    result = "";
    for (size_t i = 0; i < components.size(); ++i)
    {
        result += "/" + components[i];
    }
    
    return result;
}
