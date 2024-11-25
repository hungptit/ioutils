include(FetchContent)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG v3.6.2
  GIT_SHALLOW TRUE)

set(BUILD_TESTING OFF)
set(glaze_BUILD_EXAMPLES OFF)
set(glaze_DEVELOPER_MODE OFF)
set(glaze_ENABLE_FUZZING OFF)

FetchContent_MakeAvailable(glaze)
