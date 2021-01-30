ENABLE_TESTING()

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_cxx_compiler_flag(-static)


set(COMMAND_SRC_FILES read_data tempdir utilities writer)
foreach (test_executable ${COMMAND_TEST_EXECUTABLES})
  add_executable(${test_executable} ${test_executable}.cpp)
  target_link_libraries(${test_executable})
  target_include_directories(${test_executable} PRIVATE "${ROOT_DIR}/_deps/doctest-src/")
  add_test(${test_executable} ./${test_executable})
endforeach (test_executable)

set(COMMAND_TEST_EXECUTABLES search read_stdin)
foreach (test_executable ${COMMAND_TEST_EXECUTABLES})
  add_executable(${test_executable} ${test_executable}.cpp)
  target_link_libraries(${test_executable} ${LIB_HS} ${LIB_HS_RUNTIME})
  target_include_directories(${test_executable} PRIVATE "${ROOT_DIR}/_deps/doctest-src/")
  add_test(${test_executable} ./${test_executable})
endforeach (test_executable)
