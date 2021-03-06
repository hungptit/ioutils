# clara
FetchContent_Declare(
  clara
  GIT_REPOSITORY https://github.com/catchorg/Clara.git
  GIT_TAG v1.1.5
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(clara)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# Build using static libraries to reduce dependency.
add_cxx_compiler_flag(-static)
add_cxx_compiler_flag(-lto)
add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

set(COMMANDS fast-updatedb fast-locate fast-find fast-wc)
foreach(COMMAND ${COMMANDS})
  add_executable(${COMMAND} ${COMMAND}.cpp)
  target_link_libraries(${COMMAND} ioutils ${LIB_HS} ${LIB_HS_RUNTIME})
  target_include_directories(${COMMAND} PRIVATE "${clara_SOURCE_DIR}/include/")
  target_include_directories(${COMMAND} PRIVATE "${fmt_SOURCE_DIR}/include/")
  target_include_directories(${COMMAND} PRIVATE "${utils_SOURCE_DIR}/src/")
  target_include_directories(${COMMAND} PRIVATE "${PROJECT_SOURCE_DIR}")
  target_include_directories(${COMMAND} PRIVATE "${ROOT_DIR}/3p/include")
endforeach(COMMAND)

install_programs("/bin/" FILES ${COMMANDS})
