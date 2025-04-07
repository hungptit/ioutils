# Use LLVM toolchain by default
if(CMAKE_BUILD_TYPE MATCHES "Release")
  set(CMAKE_LINKER_TYPE LLD)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message("Disable TLO for non-release builds")
endif()
