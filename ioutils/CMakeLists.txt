if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_cxx_compiler_flag(-Wall)
  add_cxx_compiler_flag(-Wextra)
  add_cxx_compiler_flag(-Werror)
  add_cxx_compiler_flag(-pedantic)
endif()

add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

set(SRC_FILES
    "random_string.cpp"
    "temporary_dir.cpp"
    "find_parameters.cpp"
    "filesystem.cpp"
    utilities.cpp
    locate.cpp
    updatedb.cpp)
add_library(ioutils STATIC ${SRC_FILES})
target_link_libraries(ioutils PUBLIC ${LIB_HS} ${LIB_HS_RUNTIME})
target_include_directories(
  ioutils PRIVATE "${PROJECT_SOURCE_DIR}" "${ROOT_DIR}/3p/include"
                  "${fmt_SOURCE_DIR}/include/" "${utils_SOURCE_DIR}/src")
target_code_coverage(ioutils AUTO ALL)
