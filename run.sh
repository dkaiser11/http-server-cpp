#!/bin/bash

set -e

cd "$(dirname "$0")"
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build

# Run tests
echo "Running tests..."
cd build
ctest --output-on-failure
cd ..

exec ./build/server