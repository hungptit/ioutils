#!/bin/bash
set -euo pipefail                               # Use Bash strict mode

# Parse input arguments
PKGNAME=$1
EXTRA_CONFIG_OPTIONS=${2:-""}
EXTRA_MAKE_OPTIONS=${3:-""}

# Setup build configurations
source ./get_build_options.sh
APKG_SRC="$SRC_DIR/$PKGNAME"
APKG_BUILD_FOLDER="$TMP_DIR/$PKGNAME"
APKG_PREFIX="$PREFIX"

echo "====> Src folder: " "$APKG_SRC"
echo "====> Build folder: " "$APKG_BUILD_FOLDER"
echo "====> Prefix folder: " "$APKG_PREFIX"

# Build a given package
echo "====> Build folder: " "$APKG_BUILD_FOLDER"
rm -rf "$APKG_BUILD_FOLDER"
mkdir "$APKG_BUILD_FOLDER"

pushd "$APKG_BUILD_FOLDER"

if [[ -z "$EXTRA_CONFIG_OPTIONS" ]]; then
    "$APKG_SRC"/configure --prefix "$APKG_PREFIX"
else
    "$APKG_SRC"/configure --prefix "$APKG_PREFIX" "$EXTRA_CONFIG_OPTIONS"
fi

if [[ -z "$EXTRA_MAKE_OPTIONS" ]]; then
    make "$BUILD_OPTS"
else
    make "$BUILD_OPTS" "$EXTRA_MAKE_OPTIONS"
fi
    
make install
popd;

rm -rf "$APKG_BUILD_FOLDER"               # Cleanup build directory.
