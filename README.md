# High Performance Time Library

Currently in development phase. (Adding MultiThread Support)

Current version supports up to 1ns resolution, but 10ns resolution is recomended.

This library **only** works on **Intel Processors**.

[![Build Status](https://travis-ci.org/ralequi/hptimelib.svg?branch=main)](https://travis-ci.org/ralequi/hptimelib)
[![Coverage Status](https://coveralls.io/repos/github/ralequi/hptimelib/badge.svg)](https://coveralls.io/github/ralequi/hptimelib)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/ralequi/hptimelib/main/LICENSE.md)
[![Github Releases](https://img.shields.io/github/release/ralequi/hptimelib.svg)](https://github.com/ralequi/hptimelib/releases)
[![Github Issues](https://img.shields.io/github/issues/ralequi/hptimelib.svg)](https://github.com/ralequi/hptimelib/issues)

# Objetive / Use-case

This lib has been designed for High Performance applications with needs a High resolution time measurements where each nanosecond matters.

# How does it works / What this library provides

- The library uses the cpu-instruction counter and creates a relation with the value returned by clock_gettime(CLOCK_REALTIME).
It's important to note that the instruction counter is shared in a multi-core system (including NUMA) if the CPU flag `constant_tsc` is present.
- This library also can wait for a certain nanoseconds actively, usefull for very-acurate real-time applications.
- The library ensures strictly increasing timestamps.
- The library ensures not repeated timestamps if resolution is at least  ![](http://www.sciweavers.org/tex2img.php?eq=10%5E%7B8%7D%20&bc=Transparent&fc=Black&im=png&fs=12&ff=arev&edit=0).
- The 1.0 version api now supports multi-threading.  

# Performance comparison 
Tested on Intel(R) Xeon(R) CPU E5-2620 v3 @ 2.40GHz and Centos 7

| Function                             | Resolution | Time per call  |
| ------------------------------------ | ----------:| -----:|
| HPTimeLib                            | 1 ns     | **18 ns** |
| clock_gettime(CLOCK_REALTIME)        | 1 ns       | 23 ns |
| clock_gettime(CLOCK_REALTIME_COARSE) | 1000000 ns |  6 ns |
| gettimeofday(NULL)                   | 1000 ns    | 23 ns |

# Clock deviation with 1 ns resolution
Tested on Intel(R) Xeon(R) CPU E5-2620 v3 @ 2.40GHz and Centos 7 VS `clock_gettime(CLOCK_REALTIME)`.
Shoud notice that clock_gettime(CLOCK_REALTIME) can, some times, travel to the past.

| Seconds from last sync   | `clock_gettime(CLOCK_REALTIME)`  |
|:------------------------:| --------------------------------:|
| 0                        | - 0 us,  99 ns                   |
| 0.2                      | - 1 us, 592 ns                   |
| 0.4                      | + 0 us,  73 ns                   |
| 0.6                      | + 0 us,  92 ns                   |
| 0.8                      | + 0 us, 324 ns                   |
| 1                        | + 0 us, 359 ns                   |
| 1.2                      | + 0 us, 519 ns                   |
| 1.4                      | + 0 us, 483 ns                   |
| 1.6                      | + 0 us, 685 ns                   |
| 1.8                      | + 0 us, 782 ns                   |
| 2                        | + 0 us, 849 ns                   |
| 2.1                      | + 0 us, 938 ns                   |
| 2.3                      | - 0 us, 584 ns                   |
| 2.5                      | + 1 us, 160 ns                   |
| 2.7                      | + 1 us, 146 ns                   |
| 2.9                      | + 1 us, 359 ns                   |
| 3                        | + 1 us, 403 ns                   |
| 3.2                      | + 1 us, 538 ns                   |
| 3.4                      | + 1 us, 474 ns                   |
| 3.6                      | + 1 us, 691 ns                   |

# Known bugs/limitations

- Support multicore systems if constant_tsc cpuflag is present, if not, the process that initializes the library must not change the cpu core or the metrics will be wrong. By 
default, the lib wont compile if that flag is not present, the user should modify makefile and ensure that the processes are stable in a given core from the start of the 
application.
