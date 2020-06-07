PROJECT(IOUTILS)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Install all header files to destination folder.
if (NOT CMAKE_VERSION VERSION_LESS 3.0)
  install(DIRECTORY src/ DESTINATION include/ioutils)
endif()

# Format C++ code
file(GLOB_RECURSE CPP_SRC src/*hpp)
message(${CPP_SRC})
find_program(CLANG_FORMAT NAMES clang-format)
message(${CLANG_FORMAT})

add_custom_target( format-cpp COMMAND ${CLANG_FORMAT} -i ${CPP_SRC})

# Check shell script


