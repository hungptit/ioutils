#!/bin/bash
src=$1

data1="find.log"
data2="fast-find.log"

# Find all files in a given folder using GNU find.
echo "Find all files using GNU find"
/usr/bin/time find "$src" | sort -s > $data1

# Verify the fast-find output
printf "Find all files using fast-find\n"
/usr/bin/time fast-find "$src" --donot-ignore-git | sort -s > $data2
printf "==== Verify the output of fast-find ====\n"
diff "$data1" "$data2"
set -x
