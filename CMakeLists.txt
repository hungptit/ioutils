cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0048 NEW)
project(
  ioutils
  VERSION "1.0.0"
  DESCRIPTION "A blazing fast replacement tools for find and locate commands."
  LANGUAGES CXX)


# Use ccache by default
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# Generate the version information
find_package(Git)
if(Git_NOTFOUND)
  message(FATAL_ERROR "Cannot find the git command")
endif()
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
  OUTPUT_VARIABLE IOUTILS_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
file(WRITE "commands/version.hpp"
     "constexpr char version[] = \"${IOUTILS_VERSION}\";\n")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddCXXCompilerFlag)
include(CXXFeatureCheck)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ENABLE_ALL_WARNINGS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(POSITION_INDEPENDENT_CODE OFF)

# Customized options
option(IOUTILS_ENABLE_TESTING "Build unittests" ON)
option(IOUTILS_ENABLE_BENCHMARK "Build benchmark" ON)
option(USE_AVX2 "Support AVX2 by default" ON)

# Enable AVX2 by default
if(USE_AVX2)
  add_cxx_compiler_flag(-mavx2)
  add_cxx_compiler_flag(-DUSE_AVX2)
endif()

# Get required libraries
include(FetchContent)
include(cmake/lto.cmake)
include(cmake/hyperscan.cmake)
include(cmake/fmt.cmake)
include(cmake/utils.cmake)
include(cmake/cmake-scripts.cmake)
include(cmake/clara.cmake)
include(cmake/format_files.cmake)
include(cmake/jemalloc.cmake)
include(cmake/nanobench.cmake)
include(cmake/reflex.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/_deps/cmake_scripts-src/;")
include(c++-standards)
include(code-coverage)
include(sanitizers)
include(tools)
include(dependency-graph)

# Exclude test files from the code coverage report.
file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS unittests/*_test.cpp)
add_code_coverage_all_targets(EXCLUDE 3p _deps/ ${TEST_SRCS})

add_subdirectory(ioutils)
add_subdirectory(commands)

if(IOUTILS_ENABLE_BENCHMARK)
  add_subdirectory(benchmark)
endif()

if(IOUTILS_ENABLE_TESTING)
  add_subdirectory(unittests)
endif()

# Install all header files to destination folder.
install(DIRECTORY include/ioutils DESTINATION include)
install(TARGETS ioutils DESTINATION lib)
install(TARGETS fast-find DESTINATION bin)
install(TARGETS fast-locate DESTINATION bin)
install(TARGETS fast-updatedb DESTINATION bin)
install(TARGETS fast-wc DESTINATION bin)

enable_testing()
