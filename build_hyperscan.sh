#!/bin/bash

root_dir="$PWD"
src_dir="$root_dir/_deps/hyperscan-src"

# Build Hyperscan in a temporary folder.
tmp_dir=/tmp/build/hyperscan
rm -rf $tmp_dir
mkdir -p $tmp_dir

pushd "$tmp_dir" || exit
cmake "$src_dir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX="$root_dir/3p" -DHAVE_AVX2=TRUE -DFAT_RUNTIME=OFF -DBUILD_EXAMPLES=OF
make -j8
make install
popd || exit
