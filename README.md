# ioutils

## Why?

I need reusable and fast file search algorithms that can be used in my projects and none of the existing C/C++ can full fill my need.

# How?

All algorithms are written in modern C++ and some core functions are either tuned or optimized using SSE2 and AVX2. For example **mfind** command is faster than **GNU find** because it requires less system calls and can skip .git folder by default. The policy based design pattern has been used extensively to make all algorithms reusable and extensible without sacrificing the performance.

# Benchmark results

Below are benchmark results that I have collected in my Linux box using a fast SSD hard drive. All benchmark results are collected using a Boost library.

```
hungptit@hungptit ~/w/i/command> perf stat -r 5 ./mfind ../../3p/src/boost/ > /dev/null

 Performance counter stats for './mfind ../../3p/src/boost/' (5 runs):

         78.043470      task-clock:u (msec)       #    0.991 CPUs utilized            ( +-  3.92% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               126      page-faults:u             #    0.002 M/sec                    ( +-  0.77% )
        74,884,265      cycles:u                  #    0.960 GHz                      ( +-  3.92% )
       191,961,924      stalled-cycles-frontend:u #  256.34% frontend cycles idle     ( +-  3.97% )
       183,528,930      stalled-cycles-backend:u  #  245.08% backend cycles idle      ( +-  2.80% )
        72,211,909      instructions:u            #    0.96  insn per cycle
                                                  #    2.66  stalled cycles per insn  ( +-  0.00% )
        15,655,523      branches:u                #  200.600 M/sec                    ( +-  0.00% )
           163,363      branch-misses:u           #    1.04% of all branches          ( +-  2.18% )

       0.078759451 seconds time elapsed                                          ( +-  3.91% )

hungptit@hungptit ~/w/i/command> perf stat -r 5 find ../../3p/src/boost/ > /dev/null

 Performance counter stats for 'find ../../3p/src/boost/' (5 runs):

        150.271225      task-clock:u (msec)       #    0.993 CPUs utilized            ( +-  2.28% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               237      page-faults:u             #    0.002 M/sec                    ( +-  0.32% )
       160,563,463      cycles:u                  #    1.068 GHz                      ( +-  1.85% )
       383,339,551      stalled-cycles-frontend:u #  238.75% frontend cycles idle     ( +-  2.23% )
       361,275,865      stalled-cycles-backend:u  #  225.01% backend cycles idle      ( +-  1.81% )
       103,169,241      instructions:u            #    0.64  insn per cycle
                                                  #    3.72  stalled cycles per insn  ( +-  0.00% )
        22,721,983      branches:u                #  151.206 M/sec                    ( +-  0.00% )
           440,318      branch-misses:u           #    1.94% of all branches          ( +-  1.63% )

       0.151353213 seconds time elapsed                                          ( +-  2.30% )

```
