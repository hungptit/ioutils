#!/bin/bash
build_opts="-j9"
set -e
pushd "$(dirname "$0")/../"
echo Linting all files...
source venv/bin/activate
cmake -DCMAKE_BUILD_TYPE=Debug ./
make format
make lint
make "$build_opts"
make test "$build_opts"
deactivate
popd
