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
endif()

add_cxx_compiler_flag(-static)
add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

set(TEST_EXECUTABLES
    search_test
    read_stdin_test
    locate_test
    read_data_test
    tempdir_test
    utilities_test
    writer_test)
foreach(TEST_EXECUTABLE ${TEST_EXECUTABLES})
  add_executable(${TEST_EXECUTABLE} ${TEST_EXECUTABLE}.cpp)
  target_link_libraries(${TEST_EXECUTABLE} PRIVATE ${LIB_HS} ${LIB_HS_RUNTIME})
  target_code_coverage(${TEST_EXECUTABLE} AUTO ALL)
  target_include_directories(
    ${TEST_EXECUTABLE}
    PRIVATE "${ROOT_DIR}/_deps/doctest-src/" "${ROOT_DIR}/include"
            "${ROOT_DIR}/3p/include" "${fmt_SOURCE_DIR}/include/"
            "${utils_SOURCE_DIR}/src/")
  add_test(ioutils_${TEST_EXECUTABLE} ./${TEST_EXECUTABLE})
endforeach(TEST_EXECUTABLE)

enable_testing()
