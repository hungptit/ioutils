#!/bin/bash
clang-tidy "$@" -checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,modernize-*,bugprone-*,performance-*,readability-*
