include(${PROJECT_SOURCE_DIR}/cmake/catch2.cmake)

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
  target_link_libraries(
    ${target} PRIVATE ioutils test_ioutils ${LIB_HS} ${LIB_HS_RUNTIME} fmt::fmt
                      Catch2::Catch2WithMain)
  target_code_coverage(${target} AUTO ALL)
  target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}
                                               ${utils_SOURCE_DIR}/src/)
  add_test(ioutils_${target} ./${target})
endforeach()

enable_testing()
