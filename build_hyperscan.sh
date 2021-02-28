#!/bin/bash
root_dir="$PWD"
src_dir="$root_dir/_deps/hyperscan-src"

# Get the number of CPUs
os_type=$(uname)
case "$os_type" in
    "Darwin")
        {
            ncpus=$(sysctl -n hw.ncpu);
        } ;;
    "Linux")
        {
            ncpus=$(grep -c ^processor /proc/cpuinfo)
        } ;;
    *)
        {
            echo "Unsupported OS, exiting"
            exit
        } ;;
esac

# Build Hyperscan in a temporary folder.
tmp_dir=/tmp/build/hyperscan
rm -rf $tmp_dir
mkdir -p $tmp_dir

pushd "$tmp_dir" || exit

# Disable FAT_RUNTIME for local build.
cmake "$src_dir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$root_dir/3p" -DFAT_RUNTIME=OFF -DHAVE_AVX2=TRUE -DBUILD_EXAMPLES=OFF "$@"
make -j8
make install

popd || exit
