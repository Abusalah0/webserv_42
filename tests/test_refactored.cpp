#include <iostream>
#include <string>
#include <cassert>
#include <vector>

// Include the header that declares normalize_path
std::string normalize_path(const std::string& path);

int main()
{
    std::cout << "Testing refactored path normalization..." << std::endl;
    
    // Basic tests
    assert(normalize_path("") == "/");
    assert(normalize_path("/") == "/");
    assert(normalize_path("/test") == "/test");
    assert(normalize_path("test") == "/test");
    std::cout << "✓ Basic path tests passed" << std::endl;
    
    // Double slashes
    assert(normalize_path("//test") == "/test");
    assert(normalize_path("/test//path") == "/test/path");
    assert(normalize_path("///test///path///") == "/test/path");
    std::cout << "✓ Double slash removal tests passed" << std::endl;
    
    // Dot components
    assert(normalize_path("/.") == "/");
    assert(normalize_path("/./test") == "/test");
    assert(normalize_path("/test/.") == "/test");
    assert(normalize_path("/test/./path") == "/test/path");
    std::cout << "✓ Dot component resolution tests passed" << std::endl;
    
    // Double dot components
    assert(normalize_path("/..") == "/");
    assert(normalize_path("/../test") == "/test");
    assert(normalize_path("/test/..") == "/");
    assert(normalize_path("/test/../path") == "/path");
    assert(normalize_path("/test/path/..") == "/test");
    assert(normalize_path("/test/path/../file") == "/test/file");
    std::cout << "✓ Double dot component resolution tests passed" << std::endl;
    
    // URL encoding
    assert(normalize_path("/test%20file") == "/test file");
    assert(normalize_path("/test%2Fpath") == "/test/path");
    assert(normalize_path("/test+file") == "/test file");
    std::cout << "✓ URL decoding tests passed" << std::endl;
    
    // Complex cases
    assert(normalize_path("/test/../path/./file") == "/path/file");
    assert(normalize_path("//test//.//path//..//file") == "/test/file");
    assert(normalize_path("/test%20path/../file%2Ehtml") == "/file.html");
    std::cout << "✓ Complex normalization tests passed" << std::endl;
    
    // Edge cases
    assert(normalize_path("/a/b/c/../../..") == "/");
    assert(normalize_path("/a/b/c/../../../d") == "/d");
    std::cout << "✓ Edge case tests passed" << std::endl;
    
    std::cout << "\n🎉 All tests passed! Your refactored path normalization is working perfectly!" << std::endl;
    return 0;
}
