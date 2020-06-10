#!/bin/bash
src=$1

data1="find.log"
data2="fast-find.log"
data3="fd.log"

set +x

# Find all files in a given folder using GNU find.
echo "Find all files using GNU find"
/usr/bin/time find "$src" | sort -s > $data1

# Find all files using fast-find
printf "Find all files using fast-find\n"
/usr/bin/time fast-find "$src" --donot-ignore-git | sort -s > $data2

# Find all files using fd
printf "Find all files using fd\n"
/usr/bin/time fd . "$src" -H --no-ignore | sort -s > $data3

# Verify the output of fast-find
printf "==== Verify the output of fast-find ====\n"
diff "$data1" "$data2"

printf "==== Verify the output of fd ====\n"
# Verify the output of fd
diff $data1 $data3

set -x
