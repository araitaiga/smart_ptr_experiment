#!/bin/bash

# Build the calculator compiler
rm -rf ./build
mkdir build
cmake -S . -B build
cmake --build build
