# High Performance Time Library

Currently in development phase. (Adding MultiThread Support)

Current version supports up to 1ns resolution.

This library **only** works on **Intel Processors**.

# Performance comparison 
tested on Intel(R) Xeon(R) CPU E5-2630 v2 and Fedora release 20 (Heisenbug)

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
