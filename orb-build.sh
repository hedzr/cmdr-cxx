#!/usr/bin/env bash

BUILD_DIR=cmake-build-debug-ubuntu-at-orb-local

# 
cmake -S . -B $BUILD_DIR -G "Ninja Multi-Config" -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build $BUILD_DIR --config=Release
cmake -E chdir $BUILD_DIR ctest -V