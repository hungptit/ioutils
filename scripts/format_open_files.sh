#!/bin/bash -eu

# Script to format open files in the repository
# Usage: ./format_open_files.sh [branch]

# Default branch to compare against
BRANCH="${1:-master}"

# Check if required tools are installed
for cmd in markdownfmt yamlfmt shfmt shellcheck cmake-format yamllint; do
  if ! command -v "$cmd" &>/dev/null; then
    echo "Error: $cmd is required but not installed"
    echo "Install with: go install github.com/shurcooL/markdownfmt@latest"
    echo "Install with: go install github.com/google/yamlfmt/cmd/yamlfmt@latest"
    echo "Install with: go install mvdan.cc/sh/v3/cmd/shfmt@latest"
    echo "Install shellcheck, yamllint, and cmake-format via your package manager"
    exit 1
  fi
done

# Get list of changed files
changed_files=$(git diff --name-only "$BRANCH")

# Format YAML files
echo "Formatting YAML files..."
echo "$changed_files" | grep -E '[.]yaml$' | while read -r file; do
  if [ -f "$file" ]; then
    echo "  Formatting $file"
    yamlfmt "$file"
    yamllint "$file"
  fi
done

# Format Markdown files
echo "Formatting Markdown files..."
echo "$changed_files" | grep -E '[.]md$' | while read -r file; do
  if [ -f "$file" ]; then
    echo "  Formatting $file"
    markdownfmt -w "$file"
  fi
done

# Format Bash files
echo "Formatting Bash files..."
echo "$changed_files" | grep -E '[.]sh$' | while read -r file; do
  if [ -f "$file" ]; then
    echo "  Formatting $file"
    shfmt -i 2 -ci -w "$file"
    shellcheck "$file"
  fi
done

# Format C++ files
echo "Formatting Bash files..."
echo "$changed_files" | grep -E '[.](hpp|h|cpp)$' | while read -r file; do
  if [ -f "$file" ]; then
    echo "  Formatting $file"
    clang-format -i "$file"
  fi
done

# Format CMake files
echo "Formatting CMake files..."
echo "$changed_files" | grep -E '([.]cmake|CMakeLists.txt)$' | while read -r file; do
  if [ -f "$file" ]; then
    echo "  Formatting $file"
    cmake-format -i "$file"
  fi
done

echo "Formatting completed successfully"
