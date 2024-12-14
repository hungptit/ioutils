#!/bin/bash
clang-tidy "$@" -checks=-*,clang-analyzer-*,modernize-*,bugprone-*,performance-*,readability-*
