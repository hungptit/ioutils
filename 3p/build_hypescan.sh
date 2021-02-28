#!/bin/bash
root_dir="$PWD"
src_dir="$root_dir/src/hyperscan"

# Figure out the number of CPU cores
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
cmake "$src_dir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$root_dir" -DFAT_RUNTIME=OFF -DHAVE_AVX2=ON
make -j"$ncpus"
make install
popd || exit
