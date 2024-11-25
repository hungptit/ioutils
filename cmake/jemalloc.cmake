FetchContent_Declare(
  jemalloc
  GIT_REPOSITORY git@github.com:jemalloc/jemalloc.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_Populate(jemalloc)
