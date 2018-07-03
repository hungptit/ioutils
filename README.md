# Introduction #

ioutils is a small and very fast file system library for Linux and Unix environments.

# Features #

* Very fast file read algorithms.

* Very fast file traversal algorithms.

* A small set of high performance file related functions and classes.

* The mfind command line utility is 2x faster than both [find](https://www.gnu.org/software/findutils/) and [fd](https://github.com/sharkdp/fd) commands.

* The mlocate command is at least 80% faster than that of GNU locate command.

# What is the different between mfind and find and/or fd?

ioutils is written as a library and mfind is its command line utility. This library is built using generic programming concepts and the Policy Based Design pattern is heavily used through out the code base. This approach helps to

* Create a flexible and reusable algorithms without sacrificing the performance.

* Reduce the number of test cases from O(MN) to O(M + N), where M is the number of algorithms and N is the number of policies or behaviors.

* One side effect of heavily template code is the binary size of [mfind](https://github.com/hungptit/ioutils/blob/master/command/mfind.cpp) is significantly larger than that of [find](https://www.gnu.org/software/findutils/) or [fd](https://github.com/sharkdp/fd).

Below are key factors that make mfind fast:

1. [mfind](https://github.com/hungptit/ioutils/blob/master/command/mfind.cpp) uses a very fast file traversal algorithms. It is consistently 2x faster than grep in all of my tests.

2. [mfind](https://github.com/hungptit/ioutils/blob/master/command/mfind.cpp) uses [hyperscan](https://github.com/intel/hyperscan "hyperscan") as the main expression engine. According to [previous performance studies](https://rust-leipzig.github.io/regex/2017/03/28/comparison-of-regex-engines/) hyperscan is the fastest C/C++ regular expression engine.

3. Core algorithms are written in modern C++. Final algorithms code are generated at compile time using specified algorithms and policies and this helps [mfind](https://github.com/hungptit/ioutils/blob/master/command/mfind.cpp) to fully take advantage of the modern C++ compiler and architecture.

# Usage #

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
hungptit@hungptit ~/working/ioutils/command $ ./mfind ../src/ -r "search\wre.*"
../src/search_regex.hpp
```

Note that mfind can accept multiple paths. Below is an example that search for a source code file from both boost and Linux kernel source code

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ time mfind -r '/\w+options.c(p)*$' ../../3p/src/boost/ /usr/src/linux-4.17.1-gentoo/
/usr/src/linux-4.17.1-gentoo/net/ipv4/ip_options.c
/usr/src/linux-4.17.1-gentoo/drivers/net/bonding/bond_options.c
/usr/src/linux-4.17.1-gentoo/tools/perf/trace/beauty/waitid_options.c
../../3p/src/boost/libs/program_options/src/positional_options.cpp

real    0m0.187s
user    0m0.064s
sys     0m0.121s
```

# Benchmark results

## Test environments ##

* CPU: Intel Core i7 920
* Memory: 24 GB
* Hard drive: Fast SSD drive and the partition is formatted using ext4.
* OS: Gentoo Linux kernel 4.17.1 with glibc-2.27 and gcc-7.3

## Test data ##

* [boost libraries](https://www.boost.org/) source code which has more than 50K source code and object files.
* Linux kernel source code with more than 78K files.

## Results ##

### Search for files in boost library source code ###

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ ./mfind -g boost
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
boost           | gnu_find        |            Null |              10 |               1 |         1.00000 |    146265.00000 |            6.84 |
boost           | fd              |            Null |              10 |               1 |         0.87407 |    127846.00000 |            7.82 |
boost           | mfind_to_consol |            Null |              10 |               1 |         0.51545 |     75392.00000 |           13.26 |
Complete.
```

### Search for files in Linux kernel source code ###

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ ./mfind -g linux_kernel
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
Group           |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
linux_kernel    | gnu_find        |            Null |              10 |               1 |         1.00000 |    139244.00000 |            7.18 |
linux_kernel    | fd              |            Null |              10 |               1 |         1.23248 |    171616.00000 |            5.83 |
linux_kernel    | mfind_to_consol |            Null |              10 |               1 |         0.69987 |     97453.00000 |           10.26 |
Complete.
```

### Search for file using a given pattern ###

Regular expression benchmark results are depended of given search patterns since all test commands use different regular expression engine. In the benchmark below we will find source code files that match this pattern **'/\w+options.c(p)*$'** in the boost library source code. Note that the benchmark results below might be biased since I have not found any good way to test both **find** and **fd** commands.

``` shell
hungptit@hungptit ~/working/ioutils/benchmark $ ./mfind -g boost_regex
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
     -----------------------------------------------------------------------------------------------------------------------------------------------
     boost_regex     | gnu_find        |            Null |              10 |               1 |         1.00000 |    152122.00000 |            6.57 |
     boost_regex     | fd              |            Null |              10 |               1 |         1.06981 |    162741.00000 |            6.14 |
     boost_regex     | mfind_to_consol |            Null |              10 |               1 |         0.52748 |     80241.00000 |           12.46 |
     Complete.
     hungptit@hungptit ~/working/ioutils/benchmark $ ./mfind -g boost
Celero
```
