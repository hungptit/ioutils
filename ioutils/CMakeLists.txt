set(SRC_FILES
    random_string.cpp
    temporary_dir.cpp
    find_parameters.cpp
    filesystem.cpp
    search.cpp
    utilities.cpp
    locate.cpp
    updatedb.cpp)
add_library(ioutils STATIC ${SRC_FILES})
target_link_libraries(ioutils PUBLIC hs::hs hs::hs_runtime)
target_include_directories(
  ioutils PUBLIC ${PROJECT_SOURCE_DIR} ${fmt_SOURCE_DIR}/include/
                 ${utils_SOURCE_DIR}/src ${hyperscan_SOURCE_DIR}/src)
target_code_coverage(ioutils AUTO ALL)
