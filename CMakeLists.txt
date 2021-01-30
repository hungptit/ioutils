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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckCXXCompilerFlag)
include(AddCXXCompilerFlag)
include(CXXFeatureCheck)

option(USE_AVX2 "Support AVX2" ON)
add_cxx_compiler_flag(-mavx2)

set(ENABLE_ALL_WARNINGS ON)

set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-DFMT_HEADER_ONLY)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Install required libraries
include(FetchContent)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

set(POSITION_INDEPENDENT_CODE ON)

# Add doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.4.4
  GIT_SHALLOW TRUE)

set(DOCTEST_WITH_TESTS OFF)
FetchContent_Populate(doctest)

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
FetchContent_Populate(fmt)

# clara
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

# celero
FetchContent_Declare(
  celero
  GIT_REPOSITORY https://github.com/DigitalInBlue/Celero.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
set (CELERO_COMPILE_DYNAMIC_LIBRARIES OFF )
set(CELERO_ENABLE_EXPERIMENTS OFF)
FetchContent_MakeAvailable(celero)

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

include_directories("_deps/fmt-src/include")
include_directories("_deps/utils-src/src")
include_directories("_deps/clara-src/include/")
include_directories("include")
include_directories("3p/include")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Hyperscan
find_library(LIB_HS libhs.a "${CMAKE_CURRENT_SOURCE_DIR}/3p/lib" NO_DEFAULT_PATH)
find_library(LIB_HS_RUNTIME libhs_runtime.a "${CMAKE_CURRENT_SOURCE_DIR}/3p/lib" NO_DEFAULT_PATH)
find_library(LIB_HS_RUNTIME libhs_runtime.a "${CMAKE_CURRENT_SOURCE_DIR}/3p/lib" NO_DEFAULT_PATH)

set(CMAKE_VERBOSE_MAKEFILE ON)

# add_subdirectory(unittests)
add_subdirectory(commands)
add_subdirectory(benchmark)
add_subdirectory(unittests)

# Install all header files to destination folder.
install(DIRECTORY include/ioutils DESTINATION include)

ENABLE_TESTING()

# Formating and linting source code
file(GLOB CPP_FILES include/*.hpp unittests/*.[ch]pp benchmark/*.[ch]pp commands/*.[ch]pp)
message("${CPP_FILES}")

find_program(CLANG_FORMAT NAMES clang-format)
add_custom_command(
  OUTPUT .fmt_cpp_stamp
  DEPENDS ${CPP_FILES}
  COMMAND ${CLANG_FORMAT} -i ${CPP_FILES}
  COMMAND touch .fmt_cpp_stamp
  COMMENT "Format C++ files with clang-format."
  VERBATIM)

file(GLOB CMAKE_FILES CMakeLists.txt src/CMakeLists.txt)
find_program(CMAKE_FORMAT NAMES cmake-format)
if(${CMAKE_FORMAT} STREQUAL "CMAKE_FORMAT-NOTFOUND")
  add_custom_command(
    OUTPUT .fmt_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND touch .fmt_cmake_stamp
    COMMENT "Skip formatting CMake files."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .fmt_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND cmake-format -i ${CMAKE_FILES}
    COMMAND touch .fmt_cmake_stamp
    COMMENT "Format CMake files with cmake-format."
    VERBATIM)
endif()

add_custom_target(
  format
  DEPENDS .fmt_cpp_stamp .fmt_cmake_stamp
  COMMENT "Format C++ and CMake files.")
