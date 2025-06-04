#!/bin/bash -e
clang-format -i ./*/*.cpp ./*/*.hpp
shfmt -w scripts/*.sh
shellcheck scripts/*.sh
markdownfmt -w README.md benchmark.md
cmake-format -i cmake/*.cmake ./*/CMakeLists.txt CMakeLists.txt
