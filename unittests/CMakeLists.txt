# Add doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.4.4)
set(DOCTEST_WITH_TESTS OFF)
FetchContent_Populate(doctest)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_cxx_compiler_flag(-Wall)
  add_cxx_compiler_flag(-Wextra)
  add_cxx_compiler_flag(-Werror)
  add_cxx_compiler_flag(-pedantic)
  add_cxx_compiler_flag(-static)
endif()

add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

# Add test library
set(SRC_FILES "test_data.cpp")
add_library(test_ioutils STATIC ${SRC_FILES})
target_include_directories(
  test_ioutils PRIVATE "${PROJECT_SOURCE_DIR}" "${ROOT_DIR}"
                       "${fmt_SOURCE_DIR}/include/")
target_code_coverage(test_ioutils AUTO ALL)

set(TEST_EXECUTABLES
    search_test
    read_stdin_test
    locate_test
    read_data_test
    stream_reader_test
    tempdir_test
    utilities_test
    writer_test)

foreach(target ${TEST_EXECUTABLES})
  add_executable(${target} ${target}.cpp)
  target_link_libraries(${target} PRIVATE ioutils test_ioutils ${LIB_HS}
                                          ${LIB_HS_RUNTIME})
  target_code_coverage(${target} AUTO ALL)
  target_include_directories(
    ${target} PRIVATE "${doctest_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
                      "${fmt_SOURCE_DIR}/include/" "${utils_SOURCE_DIR}/src/")
  add_test(ioutils_${target} ./${target})
endforeach(TEST_EXECUTABLE)

enable_testing()
