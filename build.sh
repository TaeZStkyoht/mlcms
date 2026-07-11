#!/bin/bash
mkdir build
cd build
cmake ..
make -j $(($(nproc)*3/2))
