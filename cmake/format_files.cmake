# Formating source code
file(
  GLOB_RECURSE
  CPP_FILES
  CONFIGURE_DEPENDS
  ioutils/*.[ch]pp
  unittests/*.[ch]pp
  benchmark/*.[ch]pp
  commands/*.[ch]pp)

find_program(CLANG_FORMAT NAMES clang-format)
add_custom_command(
  OUTPUT .fmt_cpp_stamp
  DEPENDS ${CPP_FILES}
  COMMAND ${CLANG_FORMAT} -i ${CPP_FILES}
  COMMAND touch .fmt_cpp_stamp
  COMMENT "Format C++ files with clang-format."
  VERBATIM)

set(CMAKE_FILES
    ${PROJECT_SOURCE_DIR}/CMakeLists.txt
    ${PROJECT_SOURCE_DIR}/benchmark/CMakeLists.txt
    ${PROJECT_SOURCE_DIR}/commands/CMakeLists.txt
    ${PROJECT_SOURCE_DIR}/ioutils/CMakeLists.txt
    ${PROJECT_SOURCE_DIR}/unittests/CMakeLists.txt
    ${PROJECT_SOURCE_DIR}/cmake/format_files.cmake
    ${PROJECT_SOURCE_DIR}/cmake/AddCXXCompilerFlag.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CXXFeatureCheck.cmake
    ${PROJECT_SOURCE_DIR}/cmake/clara.cmake
    ${PROJECT_SOURCE_DIR}/cmake/cmake-scripts.cmake
    ${PROJECT_SOURCE_DIR}/cmake/fmt.cmake
    ${PROJECT_SOURCE_DIR}/cmake/format_files.cmake
    ${PROJECT_SOURCE_DIR}/cmake/glaze.cmake
    ${PROJECT_SOURCE_DIR}/cmake/hyperscan.cmake
    ${PROJECT_SOURCE_DIR}/cmake/jemalloc.cmake
    ${PROJECT_SOURCE_DIR}/cmake/shell-check.cmake
    ${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

# Format CMake files
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

# Lint CMake files
find_program(CMAKE_LINT NAMES cmake-format)
if(${CMAKE_LINT} STREQUAL "CMAKE_LINT-NOTFOUND")
  add_custom_command(
    OUTPUT .lint_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND touch .lint_cmake_stamp
    COMMENT "Skip formatting CMake files."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .lint_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND cmake-format -i ${CMAKE_FILES}
    COMMAND touch .lint_cmake_stamp
    COMMENT "Lint CMake files with cmake-lint."
    VERBATIM)
endif()

add_custom_target(
  lint
  DEPENDS .lint_cmake_stamp
  COMMENT "Lint CMake files.")
