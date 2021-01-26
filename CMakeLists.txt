cmake_policy(SET CMP0048 NEW)
project(
  ioutils
  VERSION 0.1.0
  LANGUAGES CXX)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

set(POSITION_INDEPENDENT_CODE ON)

# Fetch required libraries

# Add doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.4.4
  GIT_SHALLOW TRUE)

set(DOCTEST_WITH_TESTS OFF)
FetchContent_MakeAvailable(doctest)

# Add benchmark
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.5.2
  GIT_SHALLOW TRUE)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TEST OFF)
FetchContent_MakeAvailable(benchmark)

# fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 7.1.3)
set(BUILD_TESTING OFF)

set(FMT_DOC OFF)
set(FMT_TEST OFF)
FetchContent_MakeAvailable(fmt)

# Add Clara
FetchContent_Declare(
  clara
  GIT_REPOSITORY https://github.com/catchorg/Clara.git
  GIT_TAG v1.1.5
  GIT_SHALLOW TRUE)
FetchContent_Populate(clara)

# utils
FetchContent_Declare(
  utils
  GIT_REPOSITORY https://github.com/hungptit/utils.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_Populate(utils)

# hyperscan
FetchContent_Declare(
  hyperscan
  GIT_REPOSITORY https://github.com/intel/hyperscan.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(hyperscan)

# Add cmake-scripts
FetchContent_Declare(
  cmake_scripts
  GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts.git
  GIT_TAG main
  GIT_SHALLOW TRUE)

FetchContent_Populate(cmake_scripts)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/_deps/cmake_scripts-src/;")
include(c++-standards)
include(code-coverage)
include(sanitizers)
include(tools)
include(dependency-graph)

add_compile_options(-pedantic)
add_compile_options(-pedantic-errors)
add_compile_options(-static)

include_directories("_deps/doctest-src/")
include_directories("_deps/clara/include")
include_directories("_deps/utils/src")

# Install all header files to destination folder.
if (NOT CMAKE_VERSION VERSION_LESS 3.0)
  install(DIRECTORY src/ DESTINATION include/ioutils)
endif()

# Format C++ code
include(cmake/clang-format.cmake)

# Check shell script
include(cmake/shell-check.cmake)
