# Webserv Parser Test Suite

This directory contains a comprehensive test suite for the webserv configuration parser.

## Structure

```
tests/
├── test_parser.cpp         # Main test program
├── Makefile               # Build configuration for tests
├── README.md             # This file
└── config_files/         # Test configuration files
    ├── basic_valid.conf           # Basic valid configuration
    ├── multiple_servers.conf      # Multiple servers test
    ├── complex_with_locations.conf # Complex configuration with locations
    ├── error_pages.conf          # Error pages configuration
    ├── redirect_pages.conf       # Redirect pages configuration
    ├── index_pages.conf          # Index pages configuration
    ├── body_size_formats.conf    # Various body size formats
    ├── listen_variations.conf    # Listen directive variations
    ├── server_names.conf         # Server names configuration
    ├── location_methods.conf     # Location methods configuration
    ├── single_server.conf        # Single server validation
    ├── two_servers.conf          # Two servers validation
    ├── three_servers.conf        # Three servers validation
    ├── missing_http.conf         # Error: missing http block
    ├── missing_open_brace.conf   # Error: missing opening brace
    ├── missing_close_brace.conf  # Error: missing closing brace
    ├── invalid_directive.conf    # Error: invalid directive
    ├── empty.conf               # Error: empty file
    ├── invalid_body_size.conf   # Error: invalid body size
    ├── minimal_valid.conf       # Minimal valid configuration
    ├── max_complexity.conf      # Maximum complexity configuration
    ├── with_comments.conf       # Configuration with comments
    └── whitespace_variations.conf # Various whitespace patterns
```

## Test Categories

### 1. Basic Functionality Tests
- **basic_valid.conf**: Tests basic valid configuration parsing
- **multiple_servers.conf**: Tests parsing multiple server blocks
- **complex_with_locations.conf**: Tests complex configuration with location blocks

### 2. Feature-Specific Tests
- **error_pages.conf**: Tests error page directive parsing
- **redirect_pages.conf**: Tests redirect directive parsing
- **index_pages.conf**: Tests index directive parsing
- **body_size_formats.conf**: Tests various client_max_body_size formats (bytes, K, M, G)
- **listen_variations.conf**: Tests various listen directive formats
- **server_names.conf**: Tests multiple server names
- **location_methods.conf**: Tests location allowed methods

### 3. Validation Tests
- **single_server.conf**: Validates exactly 1 server is parsed
- **two_servers.conf**: Validates exactly 2 servers are parsed
- **three_servers.conf**: Validates exactly 3 servers are parsed

### 4. Error Handling Tests
- **missing_http.conf**: Should fail - missing http block
- **missing_open_brace.conf**: Should fail - missing opening brace
- **missing_close_brace.conf**: Should fail - missing closing brace
- **invalid_directive.conf**: Should fail - invalid directive
- **empty.conf**: Should fail - empty file
- **invalid_body_size.conf**: Should fail - invalid body size format

### 5. Edge Case Tests
- **minimal_valid.conf**: Tests minimal valid configuration
- **max_complexity.conf**: Tests maximum complexity configuration
- **with_comments.conf**: Tests configuration with comments
- **whitespace_variations.conf**: Tests various whitespace patterns

## Running Tests

### Build and Run
```bash
# From the tests directory
make test
```

### Build Only
```bash
make
```

### Run Tests Manually
```bash
./test_parser
```

### Clean Build Files
```bash
make clean      # Remove object files
make fclean     # Remove all generated files
make re         # Clean and rebuild
```

## Test Output

The test program provides colored output:
- 🟢 **GREEN [PASS]**: Test passed successfully
- 🔴 **RED [FAIL]**: Test failed
- 🟡 **YELLOW**: Section headers and summary information
- 🔵 **CYAN**: Main headers

### Sample Output
```
=== WEBSERV PARSER TEST SUITE ===
Testing parser functionality with various configuration files

[PASS] Basic valid configuration
[PASS] Multiple servers configuration
[PASS] Complex configuration with locations
[FAIL] Invalid directive - Exception: Parse error: invalid directive
...

=== TEST SUMMARY ===
Total tests: 22
Passed: 20
Failed: 2
```

## Adding New Tests

To add new tests:

1. Create a new configuration file in `config_files/`
2. Add a test case in `test_parser.cpp` in the appropriate section
3. Update this README if adding a new category

### Example Test Addition
```cpp
test_result("New feature test", 
            test_config_file("tests/config_files/new_feature.conf"),
            "Should parse new feature correctly");
```

## Test Configuration Guidelines

### Valid Configuration Requirements
- Must start with `http {` block
- Server blocks must be inside http block
- Location blocks must be inside server blocks
- All blocks must have proper opening and closing braces
- Directives must end with semicolons

### Common Test Patterns
- **Basic structure**: http block with one server
- **Multiple elements**: Test multiple servers, locations, directives
- **Edge cases**: Minimal configurations, maximum complexity
- **Error cases**: Invalid syntax, missing elements
- **Format variations**: Different whitespace, comments

## Integration with Main Project

The test suite uses the same source files as the main webserv project, excluding only `src/main.cpp`. This ensures that tests are always run against the current parser implementation.

## Dependencies

- C++98 compiler
- Make
- All webserv parser source files
- Test configuration files

## Troubleshooting

### Common Issues
1. **Compilation errors**: Ensure all source files are present and paths are correct
2. **Test failures**: Check if parser implementation matches expected behavior
3. **Missing files**: Ensure all test configuration files exist

### Debug Mode
To add more verbose output, modify the test functions to include debug information:
```cpp
std::cout << "Testing file: " << config_path << std::endl;
```
