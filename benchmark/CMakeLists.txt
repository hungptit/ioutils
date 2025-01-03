include(${PROJECT_SOURCE_DIR}/cmake/catch2.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/celero.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/boost.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/benchmark.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/nanobench.cmake)

set(COMMAND_SRC_FILES read_data)
foreach(src_file ${COMMAND_SRC_FILES})
  add_executable(${src_file} ${src_file}.cpp)
  target_link_libraries(
    ${src_file}
    ioutils
    celero
    Boost::system
    Boost::filesystem
    Boost::iostreams
    nanobench
    Catch2::Catch2WithMain)
  target_include_directories(${src_file} PRIVATE ${utils_SOURCE_DIR}/src)
endforeach(src_file)

set(COMMAND_BENCHMARK_COMMANDS find_benchmark locate_benchmark)
foreach(BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
  add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
  target_link_libraries(${BENCHMARK} PRIVATE celero Catch2::Catch2WithMain)
  target_include_directories(
    ${BENCHMARK} PRIVATE ${benchmark_SOURCE_DIR}/include/
                         ${utils_SOURCE_DIR}/src)
endforeach(BENCHMARK)

set(COMMAND_BENCHMARK_COMMANDS filesystem_benchmark)
foreach(BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
  add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
  target_link_libraries(
    ${BENCHMARK} PRIVATE ioutils benchmark::benchmark benchmark::benchmark_main
                         Catch2::Catch2WithMain)
  target_include_directories(
    ${BENCHMARK}
    PRIVATE "${benchmark_SOURCE_DIR}/include/" "${PROJECT_SOURCE_DIR}"
            "${ROOT_DIR}/3p/include" "${utils_SOURCE_DIR}/src/")
endforeach(BENCHMARK)
