set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/3p/llfio")
set(LLFIO_INSTALL_DIR ${THIRDPARTY_DIR})
message("llfio path: ${LLFIO_INSTALL_DIR}")

find_package(Threads REQUIRED)

find_package(
  quickcpplib
  CONFIG
  REQUIRED
  NO_DEFAULT_PATH
  PATHS
  ${LLFIO_INSTALL_DIR}/lib/cmake
  ${LLFIO_INSTALL_DIR}/lib64/cmake)

find_package(
  outcome
  CONFIG
  REQUIRED
  NO_DEFAULT_PATH
  PATHS
  ${LLFIO_INSTALL_DIR}/lib/cmake
  ${LLFIO_INSTALL_DIR}/lib64/cmake)

find_package(
  llfio
  CONFIG
  REQUIRED
  NO_DEFAULT_PATH
  PATHS
  ${LLFIO_INSTALL_DIR}/lib/cmake
  ${LLFIO_INSTALL_DIR}/lib64/cmake)
