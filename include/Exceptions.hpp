/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbibers <sbibers@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:53 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/11 12:20:01 by sbibers          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>

namespace WebservExceptions
{
    class FileOpenFailure: public std::exception
    {
        const char* what() const throw();
    };
    class InvalidValue: public std::exception
    {
        const char* what() const throw();
    };
}