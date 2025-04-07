include(FetchContent)

FetchContent_Declare(
  reflex
  GIT_REPOSITORY git@github.com:hungptit/RE-flex.git
  GIT_TAG master
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(reflex)
