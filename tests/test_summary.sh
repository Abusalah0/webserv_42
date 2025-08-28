#!/bin/bash

# Webserv Parser Test Summary Script

echo "🧪 Webserv Parser Test Suite"
echo "============================="

echo ""
echo "📁 Test Directory Structure:"
echo "tests/"
echo "├── test_parser.cpp          # Main test program"
echo "├── Makefile                 # Build configuration"
echo "├── README.md               # Documentation"
echo "└── config_files/           # Test configuration files (24 files)"

echo ""
echo "⚡ Quick Commands:"
echo "cd tests && make test        # Build and run all tests"
echo "cd tests && make quick-test  # Run tests without debug output"
echo "cd tests && make clean       # Clean build files"

echo ""
echo "📊 Current Test Status:"
cd "$(dirname "$0")"
if [ -f "test_parser" ]; then
    echo "✅ Test executable exists"
    echo ""
    echo "🚀 Running tests..."
    ./test_parser 2>/dev/null | grep -E "(PASS|FAIL|\=\=\=.*SUMMARY|\d+ tests|Passed:|Failed:)"
else
    echo "❌ Test executable not found. Run 'make' first."
fi

echo ""
echo "🔧 Test Categories:"
echo "• Basic functionality tests (3)"
echo "• Feature-specific tests (7)" 
echo "• Validation tests (3)"
echo "• Error handling tests (6)"
echo "• Edge case tests (4)"

echo ""
echo "📚 For detailed information, see tests/README.md"
