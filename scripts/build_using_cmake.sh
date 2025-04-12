#!/bin/bash
set -euo pipefail # Use Bash strict mode

os=$(uname)
case "$os" in
  "Darwin")
    {
      number_of_cpus=$(sysctl -n hw.ncpu)
    }
    ;;
  "Linux")
    {
      number_of_cpus=$(grep -c ^processor /proc/cpuinfo)
    }
    ;;
  *)
    {
      echo "Unsupported OS, exiting"
      exit
    }
    ;;
esac

pkgname="$1"
root_dir="$PWD"
local_dir="$root_dir/.local"
src_dir="$root_dir/_deps/$pkgname-src"
prefix_dir="$root_dir/3p/$pkgname"

build_dir="$local_dir/build"
rm -rf "$build_dir"
mkdir -p "$build_dir"

pushd "$build_dir"
cmake "$src_dir" -DCMAKE_INSTALL_PREFIX="$prefix_dir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=20 -DCMAKE_LINKER_TYPE=LLD -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE "${@:2}"
make "-j$number_of_cpus"
make install

# Return to the original folder.
popd
