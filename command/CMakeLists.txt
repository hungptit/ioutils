PROJECT(TOOLS)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(EXTERNAL_DIR "${ROOT_DIR}/../3p")
message("ROOT_DIR: ${ROOT_DIR}")

# set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}")
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Read the git tags to determine the project version
include(GetGitVersion)
get_git_version(GIT_VERSION)

# Tell the user what versions we are using
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" VERSION ${GIT_VERSION})
message("-- Version: ${VERSION}")

# The version of the libraries
set(GENERIC_LIB_VERSION ${VERSION})
string(SUBSTRING ${VERSION} 0 1 GENERIC_LIB_SOVERSION)

include(CheckCXXCompilerFlag)
include(AddCXXCompilerFlag)
include(CXXFeatureCheck)

# Use libc++  provided by llvm
# set(CMAKE_CXX_COMPILER "clang++")
# add_cxx_compiler_flag(-stdlib=libc++)

# set(CMAKE_BUILD_TYPE Debug)

set (CMAKE_BUILD_TYPE Release)
add_cxx_compiler_flag(-O3)
add_cxx_compiler_flag(-march=native)
add_cxx_compiler_flag(-std=c++14)
add_cxx_compiler_flag(-Wall)
add_cxx_compiler_flag(-flto)
add_cxx_compiler_flag(-pedantic)
add_cxx_compiler_flag(-pedantic-errors)
add_cxx_compiler_flag(-fstrict-aliasing)
if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
  add_cxx_compiler_flag(-Wstrict-aliasing)
endif()

if (HAVE_WTHREAD_SAFETY)
  add_definitions(-DHAVE_WTHREAD_SAFETY)
  cxx_feature_check(THREAD_SAFETY_ATTRIBUTES)
endif()

add_cxx_compiler_flag(-DFMT_HEADER_ONLY)
add_cxx_compiler_flag(-DCEREAL_RAPIDJSON_HAS_CXX11_RVALUE_REFS)

ENABLE_TESTING()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include folder
include_directories ("${EXTERNAL_DIR}/include")
include_directories ("${ROOT_DIR}/src")

# Build all
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}")
set(COMMAND_SRC_FILES linestats mfind)
foreach (src_file ${COMMAND_SRC_FILES})
  ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
  TARGET_LINK_LIBRARIES(${src_file})
endforeach (src_file)
INSTALL_PROGRAMS("/bin/" FILES ${COMMAND_SRC_FILES})
