include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES
    system
    iostreams
    container
    asio
    unordered
    intrusive
    filesystem)
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_IOSTREAMS_ENABLE_ZLIB OFF)
set(BOOST_IOSTREAMS_ENABLE_BZIP2 OFF)
set(BOOST_IOSTREAMS_ENABLE_LZMA OFF)
set(BOOST_IOSTREAMS_ENABLE_ZSTD OFF)
include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(Boost)
