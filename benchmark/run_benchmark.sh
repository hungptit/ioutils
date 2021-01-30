#!/bin/sh

echo "Collect results for fast-find command"
TEST_DIR="../" ./find_benchmark --junit="find_benchmark_small.xml"
TEST_DIR="${HOME}" ./find_benchmark --junit="find_benchmark_mid.xml"
TEST_DIR="/" ./find_benchmark --junit="find_big.xml"

echo "Collect results for fast-locate command"
./locate_benchmark --junit="fast_locate.xml"
