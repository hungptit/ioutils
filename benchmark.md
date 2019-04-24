# Test environments #

* CPU: Core I7 2200 MHz
* Memory: 16 GB
* Hard drive: Fast SSD drive.
* OS: Darwin Kernel Version 18.2.0

# Finding files and folders #

## Small folder i.e boost libraries source code with more than 50K files and folders ##

### Correctness tests ###

**Results**

``` shell
MacOS:commands hdang$ find ../../3p/src/boost/ | wc
   59458   59461 4019016
MacOS:benchmark hdang$ fd . ../../3p/src/boost/ -H --no-ignore | wc
   59457   59460 3959539   
MacOS:commands hdang$ fast-find --donot-ignore-git ../../3p/src/boost/ | wc
   59457   59460 3959539
```

**Analysis**

* All three commands produce the same output. Note that the output of GNU find is off by one because it includes a given search path. 
* The output of fast-find and fd are identical.

### Manual tests ###

**GNU find**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp find ../../3p/src/boost/ | wc
real         1.18
user         0.07
sys          0.92
   1413120  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       355  page reclaims
         1  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
      1592  voluntary context switches
        43  involuntary context switches
   59458   59461 4019016
```

**fd**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fd . ../../3p/src/boost/ -H --no-ignore | wc
real         0.43
user         0.44
sys          2.20
  10924032  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
      2644  page reclaims
        35  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
      4079  voluntary context switches
     30266  involuntary context switches
   59457   59460 3959539
```

**faast-find**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fast-find --donot-ignore-git ../../3p/src/boost/ | wc
real         0.38
user         0.05
sys          0.32
   1413120  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       356  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         4  voluntary context switches
        44  involuntary context switches
   59457   59460 3959539
```

**Analysis**

* Both GNU find and fast-find use the similar amount of memory, however, fd uses 7x more memory than both GNU find and fast-find.
* fast-find is 3x faster than GNU find and 10% faster than fd in our manual tests.
* fast-find uses less system resources than GNU find i.e 3x.
* fd use 6x more system resource than that of fast-find.

### Search for all files ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g all
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
all             | gnu_find        |               0 |              20 |               1 |         1.00000 |    486319.00000 |            2.06 |
all             | fd_noignore     |               0 |              20 |               1 |         1.10689 |    538303.00000 |            1.86 |
all             | fast_find       |               0 |              20 |               1 |         0.75659 |    367943.00000 |            2.72 |
all             | fast_find_bfs   |               0 |              20 |               1 |         0.75314 |    366266.00000 |            2.73 |
Complete.
```

**Analysis**

* If we do not ignore the **.git** folder then fast-find is about 35% faster than GNU find and 47% faster than fd.
* fast-find is faster than both commands because it does not use the system file traversal algorithms.

### Skip .git folder ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g ignore_git
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
ignore_git      | gnu_find        |               0 |              20 |               1 |         1.00000 |    497075.00000 |            2.01 |
ignore_git      | fd              |               0 |              20 |               1 |         1.10455 |    549043.00000 |            1.82 |
ignore_git      | fast_find_defau |               0 |              20 |               1 |         0.49275 |    244935.00000 |            4.08 |
ignore_git      | fast_find_bfs   |               0 |              20 |               1 |         0.49387 |    245489.00000 |            4.07 |
Complete.
```

**Analysis**

* Our performance benchmark results has shown that GNU find is still faster than fd if fd can ignore git related files.
* fast-find is 2x faster than both GNU find and fd if it can ignore **.git** folder.

### Use regex to find files ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g regex
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
regex           | find            |               0 |              20 |               1 |         1.00000 |    490978.00000 |            2.04 |
regex           | fd              |               0 |              20 |               1 |         0.92083 |    452107.00000 |            2.21 |
regex           | fast_find       |               0 |              20 |               1 |         0.54193 |    266078.00000 |            3.76 |
regex           | fast_find_bfs   |               0 |              20 |               1 |         0.53615 |    263236.00000 |            3.80 |
Complete.
```

**Analysis**

* fd is about 10% faster than GNU find in this benchmark and it is expected since fd can take the advantage of multi-cores systems.
* The performance gap between fast-find and other two commands is decreased in this benchmark, however, fd is still 70% faster than fd.

# Locate files and folders #

## fast-updatedb vs GNU updatedb ##

**Runtime**

From the output of the time command we can easily see that fast-update is about 4x faster than the latest version of GNU updatedb i.e gupdatedb. The 4x improvement comes from below factors:
* fast-update uses fast-find and gupdate uses GNU find to build the database. Our previous becnhmark results have shown that fast-find is at least 1.5x faster than that of GNU find.
* The way fast-updatedb store file information database is optimized for the read/write performance.

**fast-updatedb**

``` shell
MacOS:benchmark hdang$ time fast-updatedb / -d .all
fast-find: '/.Spotlight-V100': Operation not permitted
...

real    0m24.638s
user    0m1.619s
sys     0m13.225s
```

**GNU updatedb**

``` shell
MacOS:benchmark hdang$ time gupdatedb --localpaths="/" --output=all.db
/usr/local/Cellar/findutils/4.6.0/bin/gfind: '/usr/sbin/authserver': Permission denied
/usr/local/Cellar/findutils/4.6.0/bin/gfind: '/.Spotlight-V100': Operation not permitted
...

real    1m32.091s
user    0m27.843s
sys     1m0.713s
```

**Correctness**

A simple analysis show that fast-updatedb store more paths than GNU updatedb and we can easily see that if fast-locate is 2.5x faster than GNU locate if we want to show all paths stored in the indexed databases.

``` shell
MacOS:benchmark hdang$ time glocate -d all.db . | wc
 1548086 2129057 154803280

real    0m1.479s
user    0m1.973s
sys     0m0.102s
MacOS:benchmark hdang$ time fast-locate -d .all | wc
 1725875 2399258 168627045

real    0m0.593s
user    0m0.558s
sys     0m0.117s
```

Now let pipe outputs to files and figure out the differences bettween these two databases.

``` shell
glocate -d all.db . | sort -s > 2.log
fast-locate -d .all | sort -s > 1.log
MacOS:benchmark hdang$ diff 1.log 2.log | grep '<' | wc
  177831  448078 14183329
MacOS:benchmark hdang$ diff 1.log 2.log | grep '>' | wc
      42      88    3986
```

Below are the observations:
* GNU updatedb does not index some files without user's permission. It does ask for me for my permission during the process of generating test data.
* fast-updatedb indexes all files and folders **without asking for user permission**.
* fast-updatedb have about 177K paths more than that of GNU updatedb.

## fast-locate vs GNU locate ##

### Manual benchmark ###

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fast-locate -d .database_big 'zstd/.*doc/README[.]md$'
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
/Users/hdang/working/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/3p/src/zstd/doc/README.md
real         0.16
user         0.12
sys          0.03
   3526656  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       872  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         2  voluntary context switches
         7  involuntary context switches
MacOS:benchmark hdang$ /usr/bin/time -lp glocate -d locate_db_big --regex 'zstd/.*doc/README[.]md$'
/Users/hdang/working/3p/src/zstd/doc/README.md
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/zstd/doc/README.md
real         5.35
user         5.29
sys          0.02
   1212416  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       305  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         2  voluntary context switches
      1557  involuntary context switches
```

**Analysis**

* fast-locate is 30x faster than GNU locate in tests that try to locate files in my Macbook Pro machine. 
* The memory used by fast-locate is about 3x more than that of GNU locate. It is expected because fast-locate is compiled using heavily templatized code.

### Automated benchmark results using Celero ###

In this benchmark we try to look for files that match **zstd/.*doc/README[.]md$** pattern using two folders
1. A mid-size folder which has about 50K files and folders.
2. A big folder which has more than 1.5 million files and folders.

**Mid-size folder**

``` shell
./locate_benchmark
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
mid             | gnu_locate      |               0 |              10 |               1 |         1.00000 |    669958.00000 |            1.49 |
mid             | fast_locate     |               0 |              10 |               1 |         0.05769 |     38652.00000 |           25.87 |
Complete.
```

**Big folder**

``` shell
./locate_benchmark
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
big             | gnu_locate      |               0 |              10 |               1 |         1.00000 |   5124920.00000 |            0.20 |
big             | fast_locate     |               0 |              10 |               1 |         0.03302 |    169206.00000 |            5.91 |
Complete.
```

**Analysis**

It is obvious that fast-locate is 10x faster than GNU locate command and the differences come from below factors:
1. fast-locate can quickly read the file information database from the database faster.
2. fast-locate regular expression matching algorithm is much faster that that that of GNU locate.
3. fast-locate source code is heavily templatized and many logics or code paths have been dispatched at compile time.
