# High Performance Timing Library

Currently in development phase. (Making it user-friendly)

Current version works fine with 100ns resolution, but it will support up to 1ns without losing performance in the next commits.

This library **only** works on **Intel Processors**.

# Performance comparison (tested on Intel(R) Xeon(R) CPU E5-2630 v2):

| Function                             | Resolution | Time per call  |
| ------------------------------------ | ----------:| -----:|
| HPTimeLib                            | 100 ns     | **22 ns** |
| clock_gettime(CLOCK_REALTIME)        | 1 ns       | 25 ns |
| clock_gettime(CLOCK_REALTIME_COARSE) | 1000000 ns |  7 ns |
| gettimeofday(NULL)                   | 1000 ns    | 26 ns |
