#!/bin/bash -ex
clang-format -i ioutils/*.cpp ioutils/*.hpp
clang-format -i unittests/*.cpp unittests/*.hpp
clang-format -i commands/*.cpp commands/*.hpp

shfmt scripts/*.sh
shellcheck scripts/*.sh

cmake-format -i cmake/*.cmake CMakeLists.txt unittests/CMakeLists.txt commands/CMakeLists.txt ioutils/CMakeLists.txt
