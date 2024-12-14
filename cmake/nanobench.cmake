include(FetchContent)

FetchContent_Declare(
  nanobench
  GIT_REPOSITORY git@github.com:martinus/nanobench.git
  GIT_TAG master
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(nanobench)
