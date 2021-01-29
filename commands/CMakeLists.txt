if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Build using static libraries to reduce dependency.
add_cxx_compiler_flag(-static)
add_cxx_compiler_flag(-lto)

set(SRC_FILES fast-updatedb fast-locate fast-find fast-wc)
foreach (src_file ${SRC_FILES})
  add_executable(${src_file} ${src_file}.cpp)
  target_link_libraries(${src_file} ${LIB_HS} ${LIB_HS_RUNTIME})
endforeach (src_file)

install_programs("/bin/" FILES ${SRC_FILES})
