FetchContent_Declare(
  celero
  GIT_REPOSITORY git@github.com:hungptit/Celero.git
  GIT_TAG hd-fix-warnings)
set(CELERO_COMPILE_DYNAMIC_LIBRARIES OFF)
set(CELERO_ENABLE_EXPERIMENTS OFF)
FetchContent_MakeAvailable(celero)
