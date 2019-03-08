# Introduction #

ioutils is a small and very fast file system library for Linux and Unix environments.

# Features #

* Very fast file read algorithms.

* Very fast file traversal algorithms.

* A small set of high performance file related functions and classes.

* The mfind command line utility is faster than both [find](https://www.gnu.org/software/findutils/) and [fd](https://github.com/sharkdp/fd) commands. Our performance benchmark shows that mfind is consistently 2x faster than [GNU find](https://www.gnu.org/software/findutils/) in most tests and 20%-100% faster than [fd](https://github.com/sharkdp/fd).

* The mlocate command is 15x faster than that of [GNU locate](https://www.gnu.org/software/findutils/) command.

## What is the different between ioutils and other similar open source projects such as [GNU findutils](https://www.gnu.org/software/findutils/) and [fd](https://github.com/sharkdp/fd)?##

* ioutils is written as a library so we can easily reuse it in other projects.

* Core algorithms are created using policy-based design so we can have flexible and reusable algorithms without sacrificing the performance i.e all classes are generated at the compile time.

* Can reduce the number of test cases from O(MN) to O(M + N), where M is the number of algorithms and N is the number of policies or behaviors.

# Usage #

## Precompiled binaries ##

**All precompiled binaries for Linux and MacOS can be downloaded from** [**this github repostory**](https://github.com/hungptit/tools).

## Search for all files in given folders ##

``` shell
hungptit@hungptit ~/working/ioutils/command $ ./mfind ../src/
../src/fdwriter.hpp
../src/reader.hpp
../src/mlocate.hpp
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
hungptit@hungptit ~/working/ioutils/command $ ./mfind ../src/ -e "search\wre.*"
../src/search_regex.hpp
```

Note that mfind can accept multiple paths. Below is an example that search for a source code file from both boost and Linux kernel source code

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ time mfind -e '/\w+options.c(p)*$' ../../3p/src/boost/ /usr/src/linux-4.17.1-gentoo/
/usr/src/linux-4.17.1-gentoo/net/ipv4/ip_options.c
/usr/src/linux-4.17.1-gentoo/drivers/net/bonding/bond_options.c
/usr/src/linux-4.17.1-gentoo/tools/perf/trace/beauty/waitid_options.c
../../3p/src/boost/libs/program_options/src/positional_options.cpp

real    0m0.187s
user    0m0.064s
sys     0m0.121s
```

*Note: mfind does support caseless matching using ignore-case flag.*

## Use --level to control the level of exploration i.e BFS depth ##

``` shell
hdang@ATH020224 ~/w/i/commands> mfind ../ --level 1 -e '[.]cpp$'
../benchmark/read_data.cpp
../benchmark/mfind.cpp
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
../commands/mupdatedb.cpp
../commands/mwc.cpp
../commands/mfind.cpp
../commands/mlocate.cpp
../commands/linestats.cpp
```

## Search for files that do not match the given pattern ##
mfind does allow users to search for files that do not match a given option by turn on the **inverse-match** flag. Below is an example
``` shell
hdang@dev115 ~/w/i/command> ./mfind . -e '(o|bin|cmake|make|txt|internal|includecache|tmp|out)$|cache|CMakeFiles' -u
./linestats.cpp
./linestats
./compile_commands.json
./Makefile
./mfind
./mupdatedb
./mfind.cpp
./mlocate
./mlocate.cpp
./mupdatedb.cpp
./.database
./foo.bi
```

## Build file information database ##
Before using mlocate command we do need to build the file information database for our interrested folders.  Below command will build file information database for boost, hyperscan, tbb, and seastar packages.

``` shell
mupdatedb boost/ hyperscan/ tbb/ rocksdb/ seastar/ -v
```
## Locate files using regular expression ##

Assume we have already built the file information database using mupdatedb command then we can use mlocate to look for files that match our desired pattern.

This example will seach for all files with h and hh extensions

``` shell
mlocate '/\wfuture.(h|hh)$'
```

Or we can display all files in our database by executing **mlocate** command

``` shell
mlocate
```

# Benchmark results

## Test environments ##

* CPU: Core I7 2200 MHz
* Memory: 16 GB
* Hard drive: Fast SSD drive.
* OS: Darwin Kernel Version 18.2.0

## Test data ##

* [boost libraries](https://www.boost.org/) source code which has more than 50K source code and object files.

## What do we benchmark? ##

* We use [Celero](https://github.com/DigitalInBlue/Celero) to benchmark all test functions.
* All commands i.e find, mfind, and fd are executed using std::system function and the output is piped to a temporary file. We need to pipe to a file instead of **/dev/null** to avoid any short circuit optimization.

## Results ##

### Locate files ###

**Summary**
* mlocate is about 15x faster thatm GNU locate.
* The performance benchmark results are consistent in both MacOS and Linux environments.

``` shell
./locate_benchmark
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
regex           | gnu_locate      |               0 |              10 |               1 |         1.00000 |    353027.00000 |            2.83 |
regex           | mlocate         |               0 |              10 |               1 |         0.06850 |     24182.00000 |           41.35 |
Complete.
```

### Search for files in boost library source code ###

#### Search for all files ####

**mfind** is about 2x faster than both **GNU find** and **fd** in this performance benchmark. Note that both **mfind** and **fd** skip the **.git** folder, however, GNU find does not.
* fd performance is similar to that of GNU find.

``` shell
./mfind -g big_folder
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
big_folder      | gnu_find        |               0 |              10 |               1 |         1.00000 |    492828.00000 |            2.03 |
big_folder      | fd              |               0 |              10 |               1 |         1.00553 |    495551.00000 |            2.02 |
big_folder      | mfind_default   |               0 |              10 |               1 |         0.53460 |    263467.00000 |            3.80 |
big_folder      | mfind_dfs       |               0 |              10 |               1 |         0.52191 |    257211.00000 |            3.89 |
Complete.
```

#### Search for files using given pattern ####
Regular expression benchmark results are depended of given search patterns since all test commands use different regular expression engine. In the benchmark below we will find source code files that match this pattern **'/\w+options.c(p)*$'** in the boost library source code. Note that the benchmark results below might be biased since I have not found any good way to test both **find** and **fd** commands.

Again mfind is the fastest command in this benchmark. The results are consistent with the find all performance benchmark. The performance results are affected by the host operating systems and I/O speed, however, I get consistent results in all tested platforms.

``` shell
./mfind -g big_folder_regex
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
big_folder_rege | gnu_find        |               0 |              10 |               1 |         1.00000 |    481115.00000 |            2.08 |
big_folder_rege | fd              |               0 |              10 |               1 |         0.89113 |    428734.00000 |            2.33 |
big_folder_rege | mfind_default   |               0 |              10 |               1 |         0.54405 |    261750.00000 |            3.82 |
big_folder_rege | mfind_dfs       |               0 |              10 |               1 |         0.53777 |    258728.00000 |            3.87 |
Complete.
```
