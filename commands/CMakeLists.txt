# clara
FetchContent_Declare(
  clara
  GIT_REPOSITORY https://github.com/catchorg/Clara.git
  GIT_TAG v1.1.5
  GIT_SHALLOW TRUE)
FetchContent_Populate(clara)# clara

FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_Populate(argparse)

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# Build using static libraries to reduce dependency.
add_cxx_compiler_flag(-static)
add_cxx_compiler_flag(-lto)
add_cxx_compiler_flag(-DFMT_HEADER_ONLY)

set(COMMANDS fast-updatedb fast-locate fast-find fast-wc)
foreach(COMMAND ${COMMANDS})
  add_executable(${COMMAND} ${COMMAND}.cpp)
  target_link_libraries(${COMMAND} ioutils)
  target_include_directories(${COMMAND} PRIVATE ${clara_SOURCE_DIR}/include/
    ${fmt_SOURCE_DIR}/include
    ${argparse_SOURCE_DIR}/include
    ${utils_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}
    ${ROOT_DIR}/3p/include)
endforeach(COMMAND)

install_programs("/bin/" FILES ${COMMANDS})
