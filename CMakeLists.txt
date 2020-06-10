PROJECT(IOUTILS)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Install all header files to destination folder.
if (NOT CMAKE_VERSION VERSION_LESS 3.0)
  install(DIRECTORY src/ DESTINATION include/ioutils)
endif()

# Format C++ code
include(cmake/clang-format.cmake)

# Check shell script
include(cmake/shell-check.cmake)
