include(${PROJECT_SOURCE_DIR}/cmake/fmt.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/catch2.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/celero.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/boost.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/nanobench.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/llfio.cmake)

set(COMMAND_SRC_FILES read_data find_benchmark locate_benchmark
                      filesystem_benchmark)
foreach(src_file ${COMMAND_SRC_FILES})
  add_executable(${src_file} ${src_file}.cpp)
  target_link_libraries(
    ${src_file}
    fmt::fmt
    ioutils
    celero
    llfio::hl
    Boost::system
    Boost::filesystem
    Boost::iostreams
    nanobench
    Catch2::Catch2WithMain)
  target_include_directories(${src_file} PRIVATE ${utils_SOURCE_DIR}/src)
endforeach(src_file)
