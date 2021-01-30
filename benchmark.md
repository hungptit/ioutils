# Test environments #

## CPU ##

``` text
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   43 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       AuthenticAMD
CPU family:                      23
Model:                           113
Model name:                      AMD Ryzen 7 3700X 8-Core Processor
Stepping:                        0
CPU MHz:                         4198.712
BogoMIPS:                        8399.45
Virtualization:                  AMD-V
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        4 MiB
L3 cache:                        32 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     Not affected
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Full AMD retpoline, IBPB conditional, STIBP always-on, RSB filling
Vulnerability Srbds:             Not affected
Vulnerability Tsx async abort:   Not affected
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt pdpe1gb rdtscp lm constant_tsc rep_good nopl nonstop_tsc cpuid extd_api
                                 cid aperfmperf pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 movbe popcnt aes xsave avx f16c rdrand lahf_lm cmp_legacy svm extapic cr8_legacy abm sse4a misalignsse 3dnowprefetch osvw ibs skinit
                                 wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc mwaitx cpb cat_l3 cdp_l3 hw_pstate sme ssbd mba sev ibpb stibp vmmcall sev_es fsgsbase bmi1 avx2 smep bmi2 cqm rdt_a rdseed adx smap clflusho
                                 pt clwb sha_ni xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local clzero irperf xsaveerptr rdpru wbnoinvd arat npt lbrv svm_lock nrip_save tsc_scale vmcb_clean flushbyas
                                 id decodeassists pausefilter pfthreshold avic v_vmsave_vmload vgif umip rdpid overflow_recov succor smca
```

## OS ##

``` text
Linux hung-hien 5.10.11-gentoo-ryzen3-opt #1 SMP Fri Jan 29 18:47:37 EST 2021 x86_64 AMD Ryzen 7 3700X 8-Core Processor AuthenticAMD GNU/Linux
```

## Versions ##

**GNU find**

``` text
find --version
find (GNU findutils) 4.8.0
Packaged by Gentoo (4.8.0)
Copyright (C) 2021 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by Eric B. Decker, James Youngman, and Kevin Dalley.
Features enabled: D_TYPE O_NOFOLLOW(enabled) LEAF_OPTIMISATION FTS(FTS_CWDFD) CBO(level=2)
```

**fd**

``` text
fd --version
fd 8.2.1
```

**fast-find (master)**

``` text
fast-find --version
fast-find  5c076f39224d562c1bc880e81928108344052003
```

## Notes ##

* All automated performance benchmark results are collected on a SSD drive with a warm up cache.
* The regular expression is selected using my normal usage pattern so it is definitely biased.
* Both GNU find and fast-find are single thread so the CPU clock does not jump during the test, however, **fd does push all CPU cores to their limits**.
* Our benchmark use the latest version of GNU find which is significantly faster than the system find command.
* We avoid to alter the default options of all tested commands if possible.

# Finding files and folders #

## Automated benchmark results ##

``` text
Collect results for fast-find command
Celero
Timer resolution: 0.001000 us
Writing JUnit results to: find_benchmark_small.xml
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|all             | find            |            Null |              10 |               1 |         1.00000 |      5075.00000 |          197.04 |
|ignore_git      | find            |            Null |              10 |               1 |         1.00000 |      5182.00000 |          192.98 |
|regex           | find            |            Null |              10 |               1 |         1.00000 |      5140.00000 |          194.55 |
|all             | fd_noignore     |            Null |              10 |               1 |         1.34798 |      6841.00000 |          146.18 |
|all             | fast_find       |            Null |              10 |               1 |         0.75409 |      3827.00000 |          261.30 |
|all             | fast_find_bfs   |            Null |              10 |               1 |         0.75074 |      3810.00000 |          262.47 |
|ignore_git      | fd              |            Null |              10 |               1 |         1.36723 |      7085.00000 |          141.14 |
|ignore_git      | fast_find_defau |            Null |              10 |               1 |         0.52354 |      2713.00000 |          368.60 |
|ignore_git      | fast_find_bfs   |            Null |              10 |               1 |         0.52451 |      2718.00000 |          367.92 |
|regex           | fd              |            Null |              10 |               1 |         1.25253 |      6438.00000 |          155.33 |
|regex           | fast_find       |            Null |              10 |               1 |         1.17257 |      6027.00000 |          165.92 |
|regex           | fast_find_bfs   |            Null |              10 |               1 |         1.17626 |      6046.00000 |          165.40 |
Completed in 00:00:00.722300
Celero
Timer resolution: 0.001000 us
Writing JUnit results to: find_benchmark_mid.xml
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|all             | find            |            Null |              10 |               1 |         1.00000 |    542076.00000 |            1.84 |
|ignore_git      | find            |            Null |              10 |               1 |         1.00000 |    541212.00000 |            1.85 |
|regex           | find            |            Null |              10 |               1 |         1.00000 |    533259.00000 |            1.88 |
|all             | fd_noignore     |            Null |              10 |               1 |         1.10360 |    598234.00000 |            1.67 |
|all             | fast_find       |            Null |              10 |               1 |         0.75300 |    408183.00000 |            2.45 |
|all             | fast_find_bfs   |            Null |              10 |               1 |         0.77576 |    420522.00000 |            2.38 |
|ignore_git      | fd              |            Null |              10 |               1 |         0.73127 |    395773.00000 |            2.53 |
|ignore_git      | fast_find_defau |            Null |              10 |               1 |         0.71349 |    386151.00000 |            2.59 |
|ignore_git      | fast_find_bfs   |            Null |              10 |               1 |         0.73633 |    398508.00000 |            2.51 |
|regex           | fd              |            Null |              10 |               1 |         0.73004 |    389303.00000 |            2.57 |
|regex           | fast_find       |            Null |              10 |               1 |         0.75423 |    402201.00000 |            2.49 |
|regex           | fast_find_bfs   |            Null |              10 |               1 |         0.77245 |    411915.00000 |            2.43 |
Completed in 00:01:00.039900
Celero
Timer resolution: 0.001000 us
Writing JUnit results to: find_big.xml
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|all             | find            |            Null |              10 |               1 |         1.00000 |   1587950.00000 |            0.63 |
|ignore_git      | find            |            Null |              10 |               1 |         1.00000 |   1586264.00000 |            0.63 |
|regex           | find            |            Null |              10 |               1 |         1.00000 |   1559277.00000 |            0.64 |
|all             | fd_noignore     |            Null |              10 |               1 |         1.14194 |   1813336.00000 |            0.55 |
|all             | fast_find       |            Null |              10 |               1 |         0.72791 |   1155885.00000 |            0.87 |
|all             | fast_find_bfs   |            Null |              10 |               1 |         0.73928 |   1173932.00000 |            0.85 |
|ignore_git      | fd              |            Null |              10 |               1 |         0.96097 |   1524360.00000 |            0.66 |
|ignore_git      | fast_find_defau |            Null |              10 |               1 |         0.71294 |   1130911.00000 |            0.88 |
|ignore_git      | fast_find_bfs   |            Null |              10 |               1 |         0.72656 |   1152511.00000 |            0.87 |
|regex           | fd              |            Null |              10 |               1 |         0.97204 |   1515683.00000 |            0.66 |
|regex           | fast_find       |            Null |              10 |               1 |         0.75321 |   1174462.00000 |            0.85 |
|regex           | fast_find_bfs   |            Null |              10 |               1 |         0.76390 |   1191138.00000 |            0.84 |
Completed in 00:03:03.057376
Collect results for fast-locate command
Celero
Timer resolution: 0.001000 us
Writing JUnit results to: fast_locate.xml
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|mid             | gnu_locate      |            Null |              10 |               5 |         1.00000 |      6492.40000 |          154.03 |
|big             | gnu_locate      |            Null |              10 |               5 |         1.00000 |    212929.40000 |            4.70 |
|mid             | fast_locate     |            Null |              10 |               5 |         0.94283 |      6121.20000 |          163.37 |
|big             | fast_locate     |            Null |              10 |               5 |         0.47221 |    100547.80000 |            9.95 |
Completed in 00:00:18.117631
```

## Manual test results ##

**Test environment**
* OS: Gentoo Linux kernel 5.10.11
* alacritty 0.7.1

The manual tests aim at the real worl situations where the found paths are displayed in the terminal.

### GNU find ###

``` shell
9/usr/bin/time find  /
...
1.43user 4.04system 0:05.52elapsed 99%CPU (0avgtext+0avgdata 9568maxresident)k
0inputs+0outputs (0major+5232minor)pagefaults 0swaps
```

### fd ###

``` shell
/usr/bin/time fd -c never . /
...
5.70user 14.02system 0:03.63elapsed 543%CPU (0avgtext+0avgdata 208012maxresident)k
0inputs+0outputs (0major+51866minor)pagefaults 0swaps
```

### fast-find ###

``` shell
/usr/bin/time ./commands/fast-find /
...
0.15user 2.50system 0:04.31elapsed 61%CPU (0avgtext+0avgdata 2412maxresident)k
0inputs+0outputs (0major+221minor)pagefaults 0swaps
```

# Summary #

The benchmark results show that 
* fast-find is 50% or more faster than GNU find in all benchmarks.
* fast-find is only faster than fd in all tests except the regex benchmark for the mid size folder.
* fd is the fastest command in the benchmarks display results in the terminal (about 17.4% faster than fast-find), however, fd use about 9x more CPU resources. fast-find uses much less CPU resource than both GNU find and fd in our benchmarks. 
* fast-locate is faster than GNU locate in all benchmarks and the gap can be 2x if there are millions of files.
