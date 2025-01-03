#!/bin/bash -ex
clang-format -i ioutils/*.cpp ioutils/*.hpp
clang-format -i unittests/*.cpp unittests/*.hpp
clang-format -i commands/*.cpp commands/*.hpp

shfmt scripts/*.sh
shellcheck scripts/*.sh
markdownfmt -w README.md benchmark.md
cmake-format -i cmake/*.cmake CMakeLists.txt unittests/CMakeLists.txt commands/CMakeLists.txt ioutils/CMakeLists.txt benchmark/CMakeLists.txt
