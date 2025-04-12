# include(FetchContent)

# FetchContent_Declare(
#   reflex
#   GIT_REPOSITORY git@github.com:hungptit/RE-flex.git
#   GIT_TAG master
#   GIT_SHALLOW TRUE)

# FetchContent_Populate(reflex)

# Find the reflex library
set(REFLEX_DIR "${PROJECT_SOURCE_DIR}/3p/reflex/")

find_package(Reflex CONFIG REQUIRED NO_DEFAULT_PATH PATHS
  ${REFLEX_DIR}/lib64/cmake)
