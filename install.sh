#!/bin/bash
set -euo pipefail
DST=$1
cmake ./ -DCMAKE_INSTALL_PREFIX="$DST"
make install
