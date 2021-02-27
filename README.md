# Introduction #

ioutils provides very fast implementations for file traversal and reading/writing algorithms.

# Features #

* Very fast file read/write algorithms.

* Very fast file traversal algorithms.

* A small set of high performance file related functions and classes.

* The fast-find command is a drop-in replacement for GNU find. This command [can be 2x faster](benchmark.md) than both [find](https://www.gnu.org/software/findutils/) and [fd](https://github.com/sharkdp/fd) commands.

* The fast-locate command is a drop-in replacement for GNU locate. This command [is 10x faster](benchmark.md) than [GNU locate](https://www.gnu.org/software/findutils/) command.

**What is the different between ioutils and other similar open source projects such as [GNU findutils](https://www.gnu.org/software/findutils/) and [fd](https://github.com/sharkdp/fd)?**

* ioutils is written as a library so we can easily reuse it in other projects, for example ioutils is used by [fastgrep](https://github.com/hungptit/fastgrep), and [codesearch](https://github.com/hungptit/tools).

* Core algorithms are templatized using policy-based design so we can have flexible and reusable algorithms without sacrificing the performance i.e all classes are generated at the compile time.

* All commands support regular expression pattern matching syntax by default.

# Usage #

## Build from source ##

You must install ragel in your macOS/Linux system to be able to build ioutils from source.

``` shell
git clone https://github.com/hungptit/ioutils.git
cd ioutils
./install $PWD/test
```

## Precompiled binaries ##

**All precompiled binaries for Linux and MacOS can be downloaded from** [**this github repostory**](https://github.com/hungptit/tools).

## Search for all files in given folders ##

``` shell
hungptit@hungptit ~/working/ioutils/command $ ./fast-find ../src/
../src/fdwriter.hpp
../src/reader.hpp
../src/fast-locate.hpp
../src/utilities.hpp
../src/read_policies.hpp
../src/search.hpp
../src/linestats.hpp
../src/search_policies.hpp
../src/filedb.hpp
../src/filesystem.hpp
../src/tbb_reader.hpp
../src/threaded_reader.hpp
../src/boost_memmap.hpp
../src/ioutils.hpp
../src/search_regex.hpp
../src/temporary_dir.hpp
../src/regex_policies.hpp
```

## Search for files that match specified regular expression pattern ##

``` shell
hungptit@hungptit ~/working/ioutils/command $ ./fast-find ../src/ -e "search\wre.*"
../src/search_regex.hpp
```

Note that fast-find can accept multiple paths. Below is an example that search for a source code file from both boost and Linux kernel source code

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ time fast-find -e '/\w+options.c(p)*$' ../../3p/src/boost/ /usr/src/linux-4.17.1-gentoo/
/usr/src/linux-4.17.1-gentoo/net/ipv4/ip_options.c
/usr/src/linux-4.17.1-gentoo/drivers/net/bonding/bond_options.c
/usr/src/linux-4.17.1-gentoo/tools/perf/trace/beauty/waitid_options.c
../../3p/src/boost/libs/program_options/src/positional_options.cpp

real    0m0.187s
user    0m0.064s
sys     0m0.121s
```

*Note: fast-find does support caseless matching using ignore-case flag.*

## Use --maxdepth to control the level of exploration ##

``` shell
hdang ~/w/i/commands> fast-find ../ --maxdepth 1 -e '[.]cpp$'
../benchmark/read_data.cpp
../benchmark/fast-find.cpp
../benchmark/file_read.cpp
../benchmark/fileio.cpp
../benchmark/filesystem.cpp
../benchmark/test_read.cpp
../benchmark/locate_benchmark.cpp
../unittests/tempdir.cpp
../unittests/read_data.cpp
../unittests/deserializers.cpp
../unittests/utilities.cpp
../unittests/read_stdin.cpp
../unittests/search.cpp
../unittests/writer.cpp
../commands/fast-updatedb.cpp
../commands/mwc.cpp
../commands/fast-find.cpp
../commands/fast-locate.cpp
../commands/linestats.cpp
```

## Search for files that do not match the given pattern ##
fast-find does allow users to search for files that do not match a given option by turn on the **inverse-match** flag. Below is an example
``` shell
hdang@dev115 ~/w/i/command> ./fast-find . -e '(o|bin|cmake|make|txt|internal|includecache|tmp|out)$|cache|CMakeFiles' -u
./linestats.cpp
./linestats
./compile_commands.json
./Makefile
./fast-find
./fast-updatedb
./fast-find.cpp
./fast-locate
./fast-locate.cpp
./fast-updatedb.cpp
./.database
./foo.bi
```

## Build file information database ##
Before using fast-locate command we do need to build the file information database for our interrested folders.  Below command will build file information database for boost, hyperscan, tbb, and seastar packages.

``` shell
fast-updatedb boost/ hyperscan/ tbb/ rocksdb/ seastar/ -v
```
## Locate files using regular expression ##

Assume we have already built the file information database using fast-updatedb command then we can use fast-locate to look for files that match our desired pattern.

This example will seach for all files with h and hh extensions

``` shell
fast-locate '/\wfuture.(h|hh)$'
```

Or we can display all files in our database by executing **fast-locate** command

``` shell
fast-locate
```

# FAQs

1. Where are benchmark results? 
This [page](benchmark.md) has performance benchmark results and analysis for GNU find, fd, and fast-find commands.

2. How can I download pre-compiled binaries? 
Portable binaries for fast-find, fast-updatedb, and fast-locate can be found [here](https://github.com/hungptit/tools).

3. I am not sure if the output of fast-find is the same as that of GNU find?

We tried our best to make sure that fast-find's output will be identical to the output of GNU find so it can be used as a drop-in replacement for GNU find. Below are the correctness test results obtained using our [verification script](https://github.com/hungptit/ioutils/blob/master/unittests/verify.sh) for a large folder with 377250 files and folders. You can run it using your desired folder to verify that fast-find will work correctly in most situations. *Note that both fast-find and fd do not output the input folder paths*.

``` shell
MacOS:unittests hdang$ ./verify.sh ~/working
Find all files using GNU find
Find all files using fast-find
Find all files using fd
==== Verify the output of fast-find ====
1d0
< /Users/hdang/working
==== Verify the output of fd ====
1d0
< /Users/hdang/working
```
