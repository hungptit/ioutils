#!/bin/bash
set -e                          # Use Bash strict mode

printf "Build hyperscan"
./build_using_cmake.sh hyperscan
