#!/bin/bash
set -euo pipefail
dst=$1
cmake ./ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX="$dst"
./build_hyperscan.sh -DFAT_RUNTIME=OFF -DCMAKE_CXX_COMPILER=clang++
make install
