/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_parser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 00:00:00 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/28 17:51:08 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

// Colors for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Test result counters
static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0;

// Function to read file content
static std::string read_file(const std::string &file_name)
{
    std::ifstream file(file_name.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Test result reporting
void test_result(const std::string &test_name, bool passed, const std::string &error_msg = "")
{
    tests_total++;
    std::cout << "[" << (passed ? GREEN "PASS" : RED "FAIL") << RESET "] " << test_name;
    if (!passed && !error_msg.empty())
        std::cout << " - " << RED << error_msg << RESET;
    std::cout << std::endl;
    
    if (passed)
        tests_passed++;
    else
        tests_failed++;
}

// Test helper function to run parser and check for exceptions
bool test_config_file(const std::string &config_path, bool should_pass = true)
{
    try {
        ServerContainer serverContainer;
        const std::string buffer = read_file(config_path);
        std::vector<t_token> tokens = tokenize_string(buffer);
        parser(tokens, serverContainer);
        return should_pass;
    } catch (const std::exception& e) {
        if (!should_pass) {
            // Expected to fail
            return true;
        }
        std::cerr << "  Exception: " << e.what() << std::endl;
        return false;
    }
}

// Test helper to validate server container contents
bool validate_server_container(const std::string &config_path, 
                             size_t expected_servers,
                             const std::string &description)
{
    (void)description; // Suppress unused parameter warning
    try {
        ServerContainer serverContainer;
        const std::string buffer = read_file(config_path);
        std::vector<t_token> tokens = tokenize_string(buffer);
        parser(tokens, serverContainer);
        
        const std::vector<Server>& servers = serverContainer.get_servers();
        if (servers.size() != expected_servers) {
            std::cerr << "  Expected " << expected_servers << " servers, got " << servers.size() << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << std::endl;
        return false;
    }
}

// Run all tests
void run_tests()
{
    std::cout << CYAN << "\n=== WEBSERV PARSER TEST SUITE ===" << RESET << std::endl;
    std::cout << YELLOW << "Testing parser functionality with various configuration files\n" << RESET << std::endl;

    // Test 1: Basic valid configuration
    test_result("Basic valid configuration", 
                test_config_file("config_files/basic_valid.conf"),
                "Should parse basic valid configuration");

    // Test 2: Multiple servers
    test_result("Multiple servers configuration", 
                test_config_file("config_files/multiple_servers.conf"),
                "Should parse multiple servers");

    // Test 3: Complex configuration with locations
    test_result("Complex configuration with locations", 
                test_config_file("config_files/complex_with_locations.conf"),
                "Should parse complex configuration");

    // Test 4: Error pages configuration
    test_result("Error pages configuration", 
                test_config_file("config_files/error_pages.conf"),
                "Should parse error pages correctly");

    // Test 5: Redirect pages configuration
    test_result("Redirect pages configuration", 
                test_config_file("config_files/redirect_pages.conf"),
                "Should parse redirect pages correctly");

    // Test 6: Index pages configuration
    test_result("Index pages configuration", 
                test_config_file("config_files/index_pages.conf"),
                "Should parse index pages correctly");

    // Test 7: Various client_max_body_size formats
    test_result("Client max body size formats", 
                test_config_file("config_files/body_size_formats.conf"),
                "Should parse different body size formats");

    // Test 8: Listen directive variations
    test_result("Listen directive variations", 
                test_config_file("config_files/listen_variations.conf"),
                "Should parse various listen formats");

    // Test 9: Server names configuration
    test_result("Server names configuration", 
                test_config_file("config_files/server_names.conf"),
                "Should parse multiple server names");

    // Test 10: Location methods configuration
    test_result("Location methods configuration", 
                test_config_file("config_files/location_methods.conf"),
                "Should parse location allowed methods");

    // Validation tests
    std::cout << YELLOW << "\nValidation Tests:" << RESET << std::endl;

    test_result("Single server validation", 
                validate_server_container("config_files/single_server.conf", 1, "single server"),
                "Should have exactly 1 server");

    test_result("Two servers validation", 
                validate_server_container("config_files/two_servers.conf", 2, "two servers"),
                "Should have exactly 2 servers");

    test_result("Three servers validation", 
                validate_server_container("config_files/three_servers.conf", 3, "three servers"),
                "Should have exactly 3 servers");

    // Error handling tests
    std::cout << YELLOW << "\nError Handling Tests:" << RESET << std::endl;

    test_result("Missing http block", 
                test_config_file("config_files/missing_http.conf", false),
                "Should fail without http block");

    test_result("Missing opening brace", 
                test_config_file("config_files/missing_open_brace.conf", false),
                "Should fail with missing opening brace");

    test_result("Missing closing brace", 
                test_config_file("config_files/missing_close_brace.conf", false),
                "Should fail with missing closing brace");

    test_result("Invalid directive", 
                test_config_file("config_files/invalid_directive.conf", false),
                "Should fail with invalid directive");

    test_result("Empty file", 
                test_config_file("config_files/empty.conf", false),
                "Should fail with empty file");

    test_result("Invalid body size format", 
                test_config_file("config_files/invalid_body_size.conf", false),
                "Should fail with invalid body size");

    // Edge cases
    std::cout << YELLOW << "\nEdge Case Tests:" << RESET << std::endl;

    test_result("Minimal valid configuration", 
                test_config_file("config_files/minimal_valid.conf"),
                "Should parse minimal valid configuration");

    test_result("Maximum complexity configuration", 
                test_config_file("config_files/max_complexity.conf"),
                "Should parse maximum complexity configuration");

    test_result("Comments in configuration", 
                test_config_file("config_files/with_comments.conf"),
                "Should handle comments in configuration");

    test_result("Whitespace variations", 
                test_config_file("config_files/whitespace_variations.conf"),
                "Should handle various whitespace patterns");
}

int main()
{
    try {
        run_tests();
        
        // Print summary
        std::cout << CYAN << "\n=== TEST SUMMARY ===" << RESET << std::endl;
        std::cout << "Total tests: " << tests_total << std::endl;
        std::cout << GREEN << "Passed: " << tests_passed << RESET << std::endl;
        std::cout << RED << "Failed: " << tests_failed << RESET << std::endl;
        
        if (tests_failed == 0) {
            std::cout << GREEN << "\nAll tests passed! 🎉" << RESET << std::endl;
            return 0;
        } else {
            std::cout << RED << "\nSome tests failed! ❌" << RESET << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << RED << "Test framework error: " << e.what() << RESET << std::endl;
        return 1;
    }
}
