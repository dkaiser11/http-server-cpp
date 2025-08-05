#!/bin/bash

# Debug build and test script for http-server-cpp

set -e  # Exit on any error

echo "=== HTTP Server C++ Debug Build ==="

# Clean previous build if requested
if [[ "$1" == "clean" ]]; then
    echo "Cleaning previous build..."
    rm -rf build/
fi

# Create build directory
mkdir -p build
cd build

# Configure with Debug build type
echo "Configuring CMake for Debug build..."
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests
echo "Running tests..."
echo "=========================="
./tests --gtest_color=yes --gtest_output="xml:test_results.xml"

echo "=========================="
echo "Debug build and test completed successfully!"
echo "Test results saved to build/test_results.xml"

# Optional: Run with verbose output
if [[ "$1" == "verbose" ]]; then
    echo ""
    echo "Running tests with verbose output..."
    echo "=========================="
    ./tests --gtest_color=yes --gtest_filter=* -v
fi
