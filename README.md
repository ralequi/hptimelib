# High Performance Time Library

Currently in development phase. (Adding MultiThread Support)

Current version supports up to 1ns resolution, but 10ns resolution is recomended.

This library **only** works on **Intel Processors**.

[![Build Status](https://travis-ci.org/ralequi/hptimelib.svg?branch=master)](https://travis-ci.org/ralequi/hptimelib)
[![Coverage Status](https://img.shields.io/coveralls/ralequi/hptimelib.svg)](https://coveralls.io/r/ralequi/hptimelib)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/ralequi/hptimelib/master/LICENSE.md)
[![Github Releases](https://img.shields.io/github/release/ralequi/hptimelib.svg)](https://github.com/ralequi/hptimelib/releases)
[![Github Issues](https://img.shields.io/github/issues/ralequi/hptimelib.svg)](https://github.com/ralequi/hptimelib/issues)

# Objetive / Use-case

This lib has been designed for High Performance applications with needs a High resolution time measurements where each nanosecond matters.

# How does it works / What this library provides

- The library uses the cpu-instruction counter and creates a relation with the value returned by clock_gettime(CLOCK_REALTIME).
It's important to note that the instruction counter is shared in a multi-core system (including NUMA) if the CPU flag `constant_tsc` is present.
- This library also can wait for a certain nanoseconds actively, usefull for very-acurate real-time applications. 

# Performance comparison 
Tested on Intel(R) Xeon(R) CPU E5-2630 v2 and Fedora release 20 (Heisenbug)

| Function                             | Resolution | Time per call  |
| ------------------------------------ | ----------:| -----:|
| HPTimeLib                            | 1 ns     | **22 ns** |
| clock_gettime(CLOCK_REALTIME)        | 1 ns       | 25 ns |
| clock_gettime(CLOCK_REALTIME_COARSE) | 1000000 ns |  7 ns |
| gettimeofday(NULL)                   | 1000 ns    | 26 ns |

# Known bugs/limitations

- Not multithread support
- Support multicore systems if constant_tsc cpuflag is present, if not, the process that initializes the library must not change the cpu core or the metrics will be wrong. By 
default, the lib wont compile if that flag is not present, the user should modify makefile and ensure that the processes are stable in a given core from the start of the 
application.
