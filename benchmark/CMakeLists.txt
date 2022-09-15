add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

# Add benchmark TODO: Do not install benchmark because users do not need it.
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TEST OFF)
FetchContent_MakeAvailable(benchmark)

# celero TODO: Do not install celero because users do not need it.
FetchContent_Declare(
  celero
  GIT_REPOSITORY git@github.com:hungptit/Celero.git
  GIT_TAG hd-fix-warnings)
set(CELERO_COMPILE_DYNAMIC_LIBRARIES OFF)
set(CELERO_ENABLE_EXPERIMENTS OFF)
FetchContent_MakeAvailable(celero)

set(COMMAND_BENCHMARK_COMMANDS find_benchmark locate_benchmark)
foreach(BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
  add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
  target_link_libraries(${BENCHMARK} PRIVATE celero)
  target_include_directories(
    ${BENCHMARK} PRIVATE "${benchmark_SOURCE_DIR}/include/"
                         "${fmt_SOURCE_DIR}/include")
endforeach(BENCHMARK)
set(COMMAND_BENCHMARK_COMMANDS filesystem_benchmark)
foreach(BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
  add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
  target_link_libraries(${BENCHMARK} PRIVATE ioutils benchmark::benchmark
                                             benchmark::benchmark_main)
  target_include_directories(
    ${BENCHMARK}
    PRIVATE "${benchmark_SOURCE_DIR}/include/" "${PROJECT_SOURCE_DIR}"
            "${ROOT_DIR}/3p/include" "${utils_SOURCE_DIR}/src/")
endforeach(BENCHMARK)

# set(COMMAND_BENCHMARK_COMMANDS file_read) foreach (BENCHMARK
# ${COMMAND_BENCHMARK_COMMANDS}) ADD_EXECUTABLE(${BENCHMARK} ${BENCHMARK}.cpp)
# TARGET_LINK_LIBRARIES(${BENCHMARK} ${Boost_LIBRARIES} -lpthread ${LIB_CELERO})
# endforeach (BENCHMARK)
