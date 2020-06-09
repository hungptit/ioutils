# Format C++ code
file(GLOB_RECURSE CPP_SRC src/*hpp)
message(${CPP_SRC})
find_program(CLANG_FORMAT NAMES clang-format1)
if (${CLANG_FORMAT})
  add_custom_target( format-cpp COMMAND ${CLANG_FORMAT} -i ${CPP_SRC})
endif()
