# Add benchmark
# TODO: Do not install benchmark because users do not need it.
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.5.2
  GIT_SHALLOW TRUE)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TEST OFF)
FetchContent_MakeAvailable(benchmark)

# celero
# TODO: Do not install celero because users do not need it.
FetchContent_Declare(
  celero
  GIT_REPOSITORY https://github.com/DigitalInBlue/Celero.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
set(CELERO_COMPILE_DYNAMIC_LIBRARIES OFF)
set(CELERO_ENABLE_EXPERIMENTS OFF)
FetchContent_MakeAvailable(celero)


# This option make sure that we use the local boost version. Note that if the
# system boost is installed then CMake might use that boost version.
# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
# set(Boost_NO_SYSTEM_PATHS FALSE)
# if (Boost_NO_SYSTEM_PATHS)
#     set(BOOST_ROOT "/usr/local/Cellar/boost/1.70.0/")
#     set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#     set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
# endif (Boost_NO_SYSTEM_PATHS)

# # message(${BOOST_ROOT})
# find_package(Boost REQUIRED system filesystem iostreams)

# # Include folder
# include_directories ("${EXTERNAL_DIR}/include")
# include_directories ("${ROOT_DIR}/src")
# include_directories ("${Boost_INCLUDE_DIRS}")

# set(COMMAND_SRC_FILES read_data)
# foreach (src_file ${COMMAND_SRC_FILES})
#   ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
#   TARGET_LINK_LIBRARIES(${src_file} ${LIB_CELERO} ${Boost_LIBRARIES})
# endforeach (src_file)

set(COMMAND_BENCHMARK_COMMANDS find_benchmark locate_benchmark)
foreach (BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
    add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
    target_link_libraries(${BENCHMARK} PRIVATE celero)
    target_include_directories(${BENCHMARK} PRIVATE "_deps/benchmark-src/include/")
endforeach (BENCHMARK
)
set(COMMAND_BENCHMARK_COMMANDS filesystem_benchmark)
foreach (BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
    add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
    target_link_libraries(${BENCHMARK} PRIVATE benchmark::benchmark benchmark::benchmark_main)
    target_include_directories(${BENCHMARK} PRIVATE "_deps/benchmark-src/include/")
    target_include_directories(${BENCHMARK} PRIVATE "${ROOT_DIR}/include")
    target_include_directories(${BENCHMARK} PRIVATE "${ROOT_DIR}/3p/include")
    target_include_directories(${BENCHMARK} PRIVATE "${utils_SOURCE_DIR}/src/")
endforeach (BENCHMARK)

# set(COMMAND_BENCHMARK_COMMANDS file_read)
# foreach (BENCHMARK ${COMMAND_BENCHMARK_COMMANDS})
#   ADD_EXECUTABLE(${BENCHMARK} ${BENCHMARK}.cpp)
#   TARGET_LINK_LIBRARIES(${BENCHMARK} ${Boost_LIBRARIES} -lpthread ${LIB_CELERO})
# endforeach (BENCHMARK)
