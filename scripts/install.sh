#!/bin/bash
set -euo pipefail
dst_dir=$1

# Use clang++ to compile code on non-Windows platforms.
cmake ./ -DCMAKE_INSTALL_PREFIX="$dst_dir" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=20 -DCMAKE_LINKER_TYPE=LLD -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE
make -j9

# Copy commands to the destination
set -x
mkdir -p "$dst_dir"
cp "$PWD/commands/fast-find" "$dst_dir/"
cp "$PWD/commands/fast-locate" "$dst_dir/"
cp "$PWD/commands/fast-updatedb" "$dst_dir/"
cp "$PWD/commands/fast-wc" "$dst_dir/"
set +x
