set(COMMANDS fast-updatedb fast-locate fast-find fast-wc)
foreach(COMMAND ${COMMANDS})
  add_executable(${COMMAND} ${COMMAND}.cpp)
  target_link_libraries(${COMMAND} ioutils ${LIB_HS} ${LIB_HS_RUNTIME} fmt::fmt
                        -static)
  target_include_directories(${COMMAND} PRIVATE "${clara_SOURCE_DIR}/include/")
  target_include_directories(${COMMAND} PRIVATE "${utils_SOURCE_DIR}/src/")
  target_include_directories(${COMMAND} PRIVATE "${PROJECT_SOURCE_DIR}")
endforeach(COMMAND)

install_programs("/bin/" FILES ${COMMANDS})
