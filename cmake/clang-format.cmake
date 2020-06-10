# Format C++ code
file(GLOB_RECURSE CPP_SRC src/*hpp)
find_program(CLANG_FORMAT NAMES clang-format)
if (CLANG_FORMAT)
  add_custom_target( format-cpp COMMAND ${CLANG_FORMAT} -i ${CPP_SRC})
else()
  message(WARNING "Cannot find clang-format")
endif()
