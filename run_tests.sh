#!/bin/bash

# SimpleChat Test Runner
# Builds and runs the complete test suite

echo "SimpleChat Test Suite Runner"
echo "============================"
echo ""

# Check dependencies
echo "Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake."
    exit 1
fi

# Create test build directory inside tests folder
echo "Setting up test build..."
mkdir -p tests/build
cd tests/build

# Configure with tests enabled
echo "Configuring with tests enabled..."
cmake -DBUILD_TESTS=ON ../..

# Build tests
echo "Building test suite..."
if command -v nproc &> /dev/null; then
    make -j$(nproc)
else
    make -j$(sysctl -n hw.ncpu)
fi

# Run tests
echo ""
echo "Running test suite..."
echo "===================="
ctest --output-on-failure --verbose

echo ""
echo "Detailed test execution:"
echo "========================"
./tests/SimpleChat_Tests

echo ""
echo "Test suite completed."