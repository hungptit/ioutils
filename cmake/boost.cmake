include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES system filesystem iostreams)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(Boost)
