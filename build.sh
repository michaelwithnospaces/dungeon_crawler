#!/bin/bash

# Compile all .cpp files with the specified flags
g++ -std=c++17 -Wall -Wextra -pedantic-errors -Weffc++ -fsanitize=undefined,address *.cpp -o a.out

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running program..."
    ./a.out
else
    echo "Compilation failed."
fi
