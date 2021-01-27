#!/bin/sh

echo "Collect results for fast-find"
TEST_DIR="../../3p/src/boost" ./fast-find_benchmark --junit="fast_find_boost.xml"
TEST_DIR="../../3p/src/llvm" ./fast-find_benchmark --junit="fast_find_llvm.xml"
TEST_DIR="../../3p/src/" ./fast-find_benchmark --junit="fast_find_src.xml"
TEST_DIR="../../3p/" ./fast-find_benchmark --junit="fast_find_3p.xml"
TEST_DIR="/home/hdang/working" ./fast-find_benchmark --junit="fast_find_working.xml"
TEST_DIR="/home/hdang/" ./fast-find_benchmark --junit="fast_find_home.xml"
TEST_DIR="/" ./fast-find_benchmark --junit="fast_find_root.xml"

echo "Collect results for fast-locate"
./locate_benchmark --junit="fast_locate.xml"
