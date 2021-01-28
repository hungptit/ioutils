ENABLE_TESTING()

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_cxx_compiler_flag(-static)

set(COMMAND_SRC_FILES read_data tempdir utilities writer)
foreach (test_executable ${COMMAND_TEST_EXECUTABLES})
  ADD_EXECUTABLE(${test_executable} ${test_executable}.cpp)
  TARGET_LINK_LIBRARIES(${test_executable})
  ADD_TEST(${test_executable} ./${test_executable})
endforeach (test_executable)

set(COMMAND_TEST_EXECUTABLES search read_stdin)
foreach (test_executable ${COMMAND_TEST_EXECUTABLES})
  ADD_EXECUTABLE(${test_executable} ${test_executable}.cpp)
  TARGET_LINK_LIBRARIES(${test_executable} ${LIB_HS} ${LIB_HS_RUNTIME})
  ADD_TEST(${test_executable} ./${test_executable})
endforeach (test_executable)
