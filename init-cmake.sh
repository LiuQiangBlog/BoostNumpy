#!/bin/bash

rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_CXX_COMPILER=`which clang++` ..