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
add_cxx_compiler_flag(-Ofast)
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

# # This option make sure that we use the local boost version. Note that if the
# # system boost is installed then CMake might use that boost version.
# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME ON)
# set(Boost_NO_SYSTEM_PATHS FALSE)
# if (Boost_NO_SYSTEM_PATHS)
#   set(BOOST_ROOT "${EXTERNAL_DIR}")
#   set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#   set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
# endif (Boost_NO_SYSTEM_PATHS)
# find_package(Boost REQUIRED program_options)

# Build all
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}")

# Hyperscan
SET(LIB_HS "${EXTERNAL_DIR}/lib/libhs.a")
SET(LIB_HS_RUNTIME "${EXTERNAL_DIR}/lib/libhs_runtime.a")

set(SRC_FILES mupdatedb mlocate mfind mwc)
foreach (src_file ${SRC_FILES})
  ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
  TARGET_LINK_LIBRARIES(${src_file} ${LIB_HS} ${LIB_HS_RUNTIME})
endforeach (src_file)
INSTALL_PROGRAMS("/bin/" FILES ${SRC_FILES})

# set(COMMAND_SRC_FILES linestats)
# foreach (src_file ${COMMAND_SRC_FILES})

#   ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
#   TARGET_LINK_LIBRARIES(${src_file} ${Boost_LIBRARIES})
# endforeach (src_file)
# INSTALL_PROGRAMS("/bin/" FILES ${COMMAND_SRC_FILES})
