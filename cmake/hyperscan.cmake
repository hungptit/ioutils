# Build hyperscan using the C++17 fork.
FetchContent_Declare(
  hyperscan
  GIT_REPOSITORY git@github.com:hungptit/hyperscan.git
  GIT_TAG develop)
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(hyperscan)
add_library(hs::hs ALIAS hs)
add_library(hs::hs_runtime ALIAS hs_runtime)
