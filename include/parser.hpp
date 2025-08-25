/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:11:23 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 17:22:56 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "tokenizer.hpp"
# include "BaseBlock.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include "ServerContainer.hpp"
# include <vector>
# include <cstring>
# include <set>
# include <map>
# include <stdexcept>
# include <cstdlib>
# include <iostream>
# include <cstddef>

/**
 * @brief Throws a parse error with the given message.
 * @param msg The error message to include in the exception.
 * @throws std::runtime_error with the provided message.
 */
void    throw_parse_error(const std::string &msg);

/**
 * @brief Ensure the token at the given position exists.
 * Throws an exception if the position is out of bounds.
 * @param tokens The vector of tokens to check.
 * @param pos The position to check.
 * @throws std::runtime_error if pos is out of bounds.
 */
void    expect_token(const std::vector<t_token> &tokens, std::size_t pos);


/**
 * @brief Check if the token is of type WORD.
 * @param t The token to check.
 * @return true if the token is a WORD, false otherwise.
 */
bool    is_word(const t_token &t);

/**
 * @brief Check if the token is of type NUMBER.
 * @param t The token to check.
 * @return true if the token is a NUMBER, false otherwise.
 */
bool    is_number(const t_token &t);

/**
 * @brief Check if the token is a semicolon.
 * @param t The token to check.
 * @return true if the token is a semicolon, false otherwise.
*/
bool    is_semicolon(const t_token &t);

/**
 * @brief Check if the token is an opening brace '{'.
 * @param t The token to check.
 * @return true if the token is an opening brace, false otherwise.
 */
bool    is_brace_open(const t_token &t);

/**
 * @brief Check if the token is a closing brace '}'.
 * @param t The token to check.
 * @return true if the token is a closing brace, false otherwise.
 */
bool    is_brace_close(const t_token &t);

/**
 * @brief Check if the token is a recognized HTTP directive.
 * Recognized directives include: error_page, autoindex, listen, root, index, client_max_body_size. 
 * @param t The token to check.
 * @return true if the token is a recognized HTTP directive, false otherwise.
 */
bool    is_http_directive(const t_token &t);


/**
 * @brief Main parser function that processes a vector of tokens
 * and populates a ServerContainer with the parsed server configurations.
 * @param tokens The vector of tokens to parse.
 * @param serverContainer The ServerContainer to populate with parsed servers.
 * @throws std::runtime_error on parse errors.
 */
void    parser(const std::vector<t_token> &tokens, ServerContainer &serverContainer);

/**
 * @brief Print the contents of a ServerContainer for debugging purposes.
 * This function iterates through each Server in the container and prints its
 * properties, including BaseBlock properties and server-specific settings.
 * @param serverContainer The ServerContainer to print.
 * @note This function is intended for debugging and may produce extensive output
 * @note thanks to claude for writing this function
 * @return void
 */
void    print_server_container(const ServerContainer &serverContainer);


/**
 * @brief Skip over a location block in the token stream.
 * Advances the position to the token after the closing brace of the location block.
 * @param tokens The vector of tokens.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief Skip over a general directive in the token stream.
 * Advances the position to the token after the semicolon ending the directive.
 * @param tokens The vector of tokens.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    skip_directive(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief Parse the tokens stream and populate the BaseBlock with directives, skipping over server blocks.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock to populate with parsed directives.
 * @param pos The starting position in the token vector.
 * @throws std::runtime_error on parse errors.
 */
void    parse_baseblock(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t pos);


/**
 * @brief parse a server block from the token stream and populate the ServerContainer.
 * @param tokens The vector of tokens to parse.
 * @param serverContainer The ServerContainer to populate with the parsed server.
 * @param baseBlock The BaseBlock containing default settings to apply to the server.
 * @param pos The starting position in the token vector.
 * @throws std::runtime_error on parse errors.
 */
void    parse_servers( const std::vector<t_token> &tokens, ServerContainer &serverContainer, BaseBlock &baseBlock, size_t pos);

/**
 * @brief parse a location block from the token stream and return a Location object.
 * @param tokens The vector of tokens to parse.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
Location    parse_location_block(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief parse the tokens stream and Store directives into the given Location object.
 * @param tokens The vector of tokens to parse.
 * @param pos The current position in the token vector (will be updated).
 * @param loc The Location object to populate with parsed directives.
 * @throws std::runtime_error on parse errors.
 */
void        store_location_directive(const std::vector<t_token> &tokens, std::size_t &pos, Location &loc);

/**
 * @brief store the root directive into the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock object to populate with the root directive.
 * @param pos The current position in the token vector.
 * @throws std::runtime_error on parse errors.
 */
void    parse_root_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos);

/**
 * @brief check and skip over a semicolon in the token stream.
 * @param tokens The vector of tokens to parse.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error if the current token is not a semicolon.
 */
void    skip_over_semicolon(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief Skip over an entire server block in the token stream.
 * Advances the position to the token after the closing brace of the server block.
 * @param tokens The vector of tokens.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void skip_server_block(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief skip over a general directive in the token stream.
 * Advances the position to the token after the semicolon ending the directive.
 * @param tokens The vector of tokens.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void skip_directive(const std::vector<t_token> &tokens, std::size_t &pos);

/**
 * @brief Skip over an entire location block in the token stream.
 * Advances the position to the token after the closing brace of the location block.
 * @param tokens The vector of tokens.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos);
/**
 * @brief store a redirect directive into the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseblock The BaseBlock object to populate with the redirect directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    parse_redirect_directive(std::vector<t_token> const &tokens, BaseBlock &baseblock, std::size_t &pos);


/**
 * @brief parse an error_page directive and store it in the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock object to populate with the error_page directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    parse_error_page_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos);


/**
 * @brief parse an index directive and store it in the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock object to populate with the index directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    parse_index_direcitive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos);

/**
 * @brief parse an auto_index directive and store it in the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock object to populate with the auto_index directive.
 * @param pos The current position in the token vector.
 * @throws std::runtime_error on parse errors.
 */
void    parse_auto_index_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos);


/**
 * @brief parse a client_max_body_size directive and store it in the given BaseBlock object.
 * @param tokens The vector of tokens to parse.
 * @param baseBlock The BaseBlock object to populate with the client_max_body_size directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void parse_client_max_body_size_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos);

/**
 * @brief store a listen directive into the given Server object.
 * @param tokens The vector of tokens to parse.
 * @param srv The Server object to populate with the listen directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void    parse_listen_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos);

/**
 * @brief store a server_name directive into the given Server object.
 * @param tokens The vector of tokens to parse.
 * @param srv The Server object to populate with the server_name directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void parse_server_name_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos);

/**
 * @brief store a limit directive into the given Location object.
 * @param tokens The vector of tokens to parse.
 * @param loc The Location object to populate with the limit directive.
 * @param pos The current position in the token vector (will be updated).
 * @throws std::runtime_error on parse errors.
 */
void parse_limit_directive(const std::vector<t_token> &tokens, Location &loc, std::size_t &pos);

#endif