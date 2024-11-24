include(FetchContent)

set(FMT_TEST OFF)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY git@github.com:fmtlib/fmt.git
  GIT_TAG 11.0.2
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(fmt)
