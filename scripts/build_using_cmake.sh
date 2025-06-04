#!/bin/bash
set -euo pipefail # Use Bash strict mode

# Default configuration
DEFAULT_BUILD_TYPE="Release"
DEFAULT_CXX_STANDARD="20"
DEFAULT_COMPILER="clang++"
DEFAULT_LINKER="LLD"
DEFAULT_GENERATOR="Ninja"

# Print usage information
usage() {
  cat <<EOF
Usage: $(basename "$0") <package_name> [options]

Build a package using CMake.

Arguments:
    package_name    Name of the package to build

Options:
    -h, --help              Show this help message
    -t, --build-type TYPE   Build type (Debug|Release|RelWithDebInfo|MinSizeRel) [default: $DEFAULT_BUILD_TYPE]
    -s, --std STD           C++ standard [default: $DEFAULT_CXX_STANDARD]
    -c, --compiler COMP     C++ compiler [default: $DEFAULT_COMPILER]
    -l, --linker LINKER     Linker [default: $DEFAULT_LINKER]
    -g, --generator GEN     CMake generator [default: $DEFAULT_GENERATOR]
    -p, --prefix DIR        Installation prefix [default: 3p/<package_name>]
    -d, --deps-dir DIR      Dependencies directory [default: _deps]
    -b, --build-dir DIR     Build directory [default: .local/build/<package_name>]
    --no-clean              Don't clean build directory before building
    --no-ipo                Disable interprocedural optimization

Additional CMake options can be passed after these options.
EOF
  exit 1
}

# Parse command line arguments
parse_args() {
  local pkgname=""
  local build_type="$DEFAULT_BUILD_TYPE"
  local cxx_std="$DEFAULT_CXX_STANDARD"
  local compiler="$DEFAULT_COMPILER"
  local linker="$DEFAULT_LINKER"
  local generator="$DEFAULT_GENERATOR"
  local clean_build=true
  local enable_ipo=true
  local cmake_args=()

  while [[ $# -gt 0 ]]; do
    case "$1" in
      -h | --help)
        usage
        ;;
      -t | --build-type)
        build_type="$2"
        shift 2
        ;;
      -s | --std)
        cxx_std="$2"
        shift 2
        ;;
      -c | --compiler)
        compiler="$2"
        shift 2
        ;;
      -l | --linker)
        linker="$2"
        shift 2
        ;;
      -g | --generator)
        generator="$2"
        shift 2
        ;;
      -p | --prefix)
        prefix_dir="$2"
        shift 2
        ;;
      -d | --deps-dir)
        deps_dir="$2"
        shift 2
        ;;
      -b | --build-dir)
        build_dir="$2"
        shift 2
        ;;
      --no-clean)
        clean_build=false
        shift
        ;;
      --no-ipo)
        enable_ipo=false
        shift
        ;;
      *)
        if [[ -z "$pkgname" ]]; then
          pkgname="$1"
        else
          cmake_args+=("$1")
        fi
        shift
        ;;
    esac
  done

  if [[ -z "$pkgname" ]]; then
    echo "Error: Package name is required"
    usage
  fi

  # Set default directories if not specified
  root_dir="${PWD}"
  deps_dir="${deps_dir:-$root_dir/_deps}"
  build_dir="${build_dir:-$root_dir/.local/build/$pkgname}"
  prefix_dir="${prefix_dir:-$root_dir/3p/$pkgname}"
  src_dir="$deps_dir/$pkgname-src"

  # Validate directories
  if [[ ! -d "$src_dir" ]]; then
    echo "Error: Source directory '$src_dir' does not exist"
    exit 1
  fi

  # Create build directory
  mkdir -p "$build_dir"

  # Clean build directory if requested
  if [[ "$clean_build" = true ]]; then
    echo "Cleaning build directory: $build_dir"
    rm -rf "$build_dir"
  fi

  # Build CMake arguments
  local cmake_cmd=(
    cmake
    -G "$generator"
    -S "$src_dir"
    -DCMAKE_INSTALL_PREFIX="$prefix_dir"
    -DCMAKE_BUILD_TYPE="$build_type"
    -DCMAKE_CXX_COMPILER="$compiler"
    -DCMAKE_CXX_STANDARD="$cxx_std"
    -DCMAKE_LINKER_TYPE="$linker"
  )

  if [[ "$enable_ipo" = true ]]; then
    cmake_cmd+=(-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE)
  fi

  # Add any additional CMake arguments
  cmake_cmd+=("${cmake_args[@]}")

  # Execute build
  echo "Building $pkgname..."
  echo "Source directory: $src_dir"
  echo "Build directory: $build_dir"
  echo "Install prefix: $prefix_dir"
  echo "Build type: $build_type"
  echo "C++ standard: $cxx_std"
  echo "Compiler: $compiler"
  echo "Linker: $linker"
  echo "Generator: $generator"
  echo "IPO enabled: $enable_ipo"

  mkdir -p "$build_dir"
  pushd "$build_dir" >/dev/null
  "${cmake_cmd[@]}"
  cmake --build "$build_dir"
  cmake --install "$build_dir"
  popd >/dev/null

  echo "Build completed successfully"
}

# Main execution
parse_args "$@"
