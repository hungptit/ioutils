#!/bin/bash
BRANCH="${1:-master}"
changed_files=$(git diff --name-only "$BRANCH")

# Run clang-tidy on open C++ files
echo "Run clang-tidy on opened C++ files..."
echo "$changed_files" | grep -E '[.](hpp|h|cpp)$' | while read -r file; do
  if [ -f "$file" ]; then
    clang-tidy "$file" -checks=-*,clang-analyzer-*,modernize-*,bugprone-*,performance-*,readability-*
  fi
done
