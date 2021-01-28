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

set(COMMAND_SRC_FILES find_benchmark locate_benchmark)
foreach (src_file ${COMMAND_SRC_FILES})
    add_executable(${src_file} ${src_file}.cpp)
    target_link_libraries(${src_file} celero)
endforeach (src_file)

set(COMMAND_SRC_FILES filesystem_benchmark)
foreach (src_file ${COMMAND_SRC_FILES})
    add_executable(${src_file} ${src_file}.cpp)
    target_link_libraries(${src_file} benchmark::benchmark benchmark::benchmark_main)
endforeach (src_file)

# set(COMMAND_SRC_FILES file_read)
# foreach (src_file ${COMMAND_SRC_FILES})
#   ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
#   TARGET_LINK_LIBRARIES(${src_file} ${Boost_LIBRARIES} -lpthread ${LIB_CELERO})
# endforeach (src_file)
