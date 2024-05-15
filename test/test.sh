#!/bin/bash

# Set the path to the C source file
SOURCE_FILE="../data/hello_world.c"

# Build the project using CMakeLists.txt
cd ..
mkdir -p cmake-build-test
cd cmake-build-test || exit 1;
cmake ..
make

# Execute the built program and capture its output
./simd_lexer "$SOURCE_FILE" > simd_lexer_output.txt

# Execute the clang command and capture its output
clang -fsyntax-only -Xclang -dump-tokens "$SOURCE_FILE" 2>&1 \
    | awk -F "'" '{print $1, $2}' \
    > clang_output.txt

echo Running on "$SOURCE_FILE":

# Check if the outputs are equal
if cmp -s "simd_lexer_output.txt" "clang_output.txt"; then
    # Outputs are equal, print PASSED in green
    echo -e "\e[32mPASSED\e[0m"
else
    # Outputs are not equal, print FAILED in red
    echo -e "\e[31mFAILED\e[0m"

    # Print differences with color highlighting
    diff --color=always "simd_lexer_output.txt" "clang_output.txt"
fi
