// Performance test in C++ with sub-ns average precision using TSC
#include <hptl.h>
#include <hptl_config.h>
#include <sys/time.h>
#include <time.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <hptl.hpp>
#include <string>
#include <vector>

#if !defined(__x86_64__) && !defined(_M_X64)
#error This performance test uses rdtsc and only supports x86_64
#endif

static inline void cpuid_barrier () {
	unsigned a, b, c, d;
	asm volatile ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(0));
}

static inline uint64_t rdtsc () {
	unsigned lo, hi;
	asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp () {
	unsigned lo, hi, aux;
	asm volatile ("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
	return ((uint64_t)hi << 32) | lo;
}

template <typename Fn>
static double measure_cycles_per_call_bracketed (uint64_t iters, Fn&& fn) {
	volatile uint64_t sink = 0;
	uint64_t sum_cycles    = 0;

	for (uint64_t i = 0; i < iters; ++i) {
		cpuid_barrier ();
		uint64_t t0 = rdtsc ();
		sink += fn ();
		uint64_t t1 = rdtscp ();
		cpuid_barrier ();
		sum_cycles += (t1 - t0);
	}

	// Baseline (same timing bracket, no-op body)
	uint64_t base_cycles = 0;
	for (uint64_t i = 0; i < iters; ++i) {
		cpuid_barrier ();
		uint64_t t0 = rdtsc ();
		asm volatile ("" ::: "memory");
		sink += 0;  // account for sink update cost
		uint64_t t1 = rdtscp ();
		cpuid_barrier ();
		base_cycles += (t1 - t0);
	}

	double per_call =
	    double (sum_cycles > base_cycles ? (sum_cycles - base_cycles) : 0ULL) / double (iters);
	(void)sink;
	return per_call;
}

static inline double cycles_to_ns (double cycles, uint64_t cycles_per_second) {
	return (cycles * 1e9) / double (cycles_per_second);
}

struct Result {
	std::string name;
	double cycles_per_call;
	double ns_per_call;
};

int main () {
	// Configure HPTL with ns precision
	hptl_config conf{};
	conf.precision  = 9;  // ns
	conf.clockspeed = 0;  // auto

	// Set up clocks/APIs
	hptl_clock clk{};
	if (hptl_initclk (&clk, &conf) != 0) {
		std::fprintf (stderr, "Failed to init hptl clock\n");
		return 1;
	}

#ifdef HPTL_ENABLE_LEGACY
	hptl_init (&conf);
#endif

	hptl::hptl cpp_clock{};  // default config inside

	// Derive cycles/sec from library (for precise ns conversion)
	const uint64_t cycles_per_sec = hptl_ns2cycles (&clk, 1000000000ULL);
	if (cycles_per_sec == 0) {
		std::fprintf (stderr, "cycles_per_sec is 0. Is HPTL_ONLYLINUXAPI enabled?\n");
		return 2;
	}

	// Iterations (tunable via env)
	uint64_t iters = 5000000ULL;  // default 5M
	if (const char* env = std::getenv ("HPTL_BENCH_ITERS")) {
		uint64_t v = std::strtoull (env, nullptr, 10);
		if (v > 0)
			iters = v;
	}

	std::vector<Result> results;
	results.reserve (16);

	// 1) HPTL C++ API
	{
		auto cycles = measure_cycles_per_call_bracketed (
		    iters, [&] () -> uint64_t { return cpp_clock.getTime (); });
		results.push_back ({"hptl (C++)", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}

	// 2) HPTL C API (pointer)
	{
		auto cycles = measure_cycles_per_call_bracketed (
		    iters, [&] () -> uint64_t { return hptl_getTime (&clk); });
		results.push_back ({"hptl (C ptr)", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}

	// 3) HPTL legacy API (global clock)
#ifdef HPTL_ENABLE_LEGACY
	{
		auto cycles =
		    measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t { return hptl_get (); });
		results.push_back ({"hptl (C legacy)", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}
#endif

	// C/C++ common time sources
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			struct timespec ts;
			clock_gettime (CLOCK_REALTIME, &ts);
			return (uint64_t)ts.tv_nsec;
		});
		results.push_back (
		    {"clock_gettime(CLOCK_REALTIME)", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			struct timespec ts;
			clock_gettime (CLOCK_REALTIME_COARSE, &ts);
			return (uint64_t)ts.tv_nsec;
		});
		results.push_back ({"clock_gettime(CLOCK_REALTIME_COARSE)",
		                    cycles,
		                    cycles_to_ns (cycles, cycles_per_sec)});
	}
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			struct timeval tv;
			gettimeofday (&tv, nullptr);
			return (uint64_t)tv.tv_usec;
		});
		results.push_back ({"gettimeofday", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}

	// C++ chrono variants
	using namespace std::chrono;
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			auto t = high_resolution_clock::now ();
			return (uint64_t)t.time_since_epoch ().count ();
		});
		results.push_back ({"std::chrono::high_resolution_clock::now()",
		                    cycles,
		                    cycles_to_ns (cycles, cycles_per_sec)});
	}
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			auto t = steady_clock::now ();
			return (uint64_t)t.time_since_epoch ().count ();
		});
		results.push_back (
		    {"std::chrono::steady_clock::now()", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}
	{
		auto cycles = measure_cycles_per_call_bracketed (iters, [&] () -> uint64_t {
			auto t = system_clock::now ();
			return (uint64_t)t.time_since_epoch ().count ();
		});
		results.push_back (
		    {"std::chrono::system_clock::now()", cycles, cycles_to_ns (cycles, cycles_per_sec)});
	}

	// Print summary (sub-ns, 0.1ns resolution)
	std::printf ("Performance (avg per call, TSC bracket) — iters=%llu\n",
	             (unsigned long long)iters);
	for (const auto& r : results) {
		std::printf ("- %-40s: %8.1f cycles | %6.1f ns\n",
		             r.name.c_str (),
		             r.cycles_per_call,
		             r.ns_per_call);
	}

	// -----------------------------
	// Bulk loop measurements
	// -----------------------------
	auto bulk_ns_per_call = [&] (auto&& fn, uint64_t n) -> uint64_t {
		volatile uint64_t sink = 0;
		hptl_t s               = hptl_getTime (&clk);
		for (uint64_t i = 0; i < n; ++i) {
			sink += fn ();
		}
		hptl_t e   = hptl_getTime (&clk);
		hptl_t dif = e - s;
		(void)sink;
		return hptl_clkntimestamp (&clk, dif) / n;
	};

	auto chrono_res_ns = [] (auto /*ClockTag*/) -> uint64_t {
		using Clock = decltype (std::chrono::high_resolution_clock ());  // unused
		return 0;                                                        // fallback
	};

	auto chrono_res_ns_steady = [] () -> uint64_t {
		using P        = std::chrono::steady_clock::period;
		long double ns = 1e9L * (long double)P::num / (long double)P::den;
		if (ns < 0)
			ns = 0;
		return (uint64_t)(ns + 0.5L);
	};
	auto chrono_res_ns_high = [] () -> uint64_t {
		using P        = std::chrono::high_resolution_clock::period;
		long double ns = 1e9L * (long double)P::num / (long double)P::den;
		if (ns < 0)
			ns = 0;
		return (uint64_t)(ns + 0.5L);
	};
	auto chrono_res_ns_system = [] () -> uint64_t {
		using P        = std::chrono::system_clock::period;
		long double ns = 1e9L * (long double)P::num / (long double)P::den;
		if (ns < 0)
			ns = 0;
		return (uint64_t)(ns + 0.5L);
	};

	uint64_t bulk_iters = 100000000ULL;  // 1e8 por defecto
	if (const char* env = std::getenv ("HPTL_BENCH_BULK_ITERS")) {
		uint64_t v = std::strtoull (env, nullptr, 10);
		if (v > 0)
			bulk_iters = v;
	}

	std::puts ("\nBulk loop measurements (HPTL timed)");
	// 1) hptl C++
	{
		volatile hptl_t last = 0;
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    last = cpp_clock.getTime ();
			    return (uint64_t)last;
		    },
		    bulk_iters);
		uint64_t res = cpp_clock.getclkres ();
		double cyc   = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "hptl (C++)                             [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)res,
		    cyc,
		    (double)ns_per_call);
	}

	// 2) hptl C ptr
	{
		volatile hptl_t last = 0;
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    last = hptl_getTime (&clk);
			    return (uint64_t)last;
		    },
		    bulk_iters);
		uint64_t res = hptl_getclkres (&clk);
		double cyc   = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "hptl (C ptr)                           [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)res,
		    cyc,
		    (double)ns_per_call);
	}

	// 3) hptl legacy
#ifdef HPTL_ENABLE_LEGACY
	{
		volatile hptl_t last = 0;
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    last = hptl_get ();
			    return (uint64_t)last;
		    },
		    bulk_iters);
		uint64_t res = hptl_getres ();
		double cyc   = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "hptl (C legacy)                        [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)res,
		    cyc,
		    (double)ns_per_call);
	}
#endif

	// clock_gettime REALTIME
	{
		struct timespec ts{};
		struct timespec resol{};
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    clock_gettime (CLOCK_REALTIME, &ts);
			    return (uint64_t)ts.tv_nsec;
		    },
		    bulk_iters);
		clock_getres (CLOCK_REALTIME, &resol);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "clock_gettime(CLOCK_REALTIME)          [%7ldns res]  %6.1f cycles  %5.1f ns/call\n",
		    resol.tv_nsec,
		    cyc,
		    (double)ns_per_call);
	}

	// clock_gettime COARSE
	{
		struct timespec ts{};
		struct timespec resol{};
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    clock_gettime (CLOCK_REALTIME_COARSE, &ts);
			    return (uint64_t)ts.tv_nsec;
		    },
		    bulk_iters);
		clock_getres (CLOCK_REALTIME_COARSE, &resol);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "clock_gettime(CLOCK_REALTIME_COARSE)   [%7ldns res]  %6.1f cycles  %5.1f ns/call\n",
		    resol.tv_nsec,
		    cyc,
		    (double)ns_per_call);
	}

	// gettimeofday
	{
		struct timeval tv{};
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    gettimeofday (&tv, nullptr);
			    return (uint64_t)tv.tv_usec;
		    },
		    bulk_iters);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "gettimeofday                           [%7ldns res]  %6.1f cycles  %5.1f ns/call\n",
		    1000L,
		    cyc,
		    (double)ns_per_call);
	}

	// chrono highs
	{
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    auto t = std::chrono::high_resolution_clock::now ();
			    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds> (
			               t.time_since_epoch ())
			        .count ();
		    },
		    bulk_iters);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "std::chrono::high_resolution_clock     [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)chrono_res_ns_high (),
		    cyc,
		    (double)ns_per_call);
	}
	{
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    auto t = std::chrono::steady_clock::now ();
			    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds> (
			               t.time_since_epoch ())
			        .count ();
		    },
		    bulk_iters);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "std::chrono::steady_clock              [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)chrono_res_ns_steady (),
		    cyc,
		    (double)ns_per_call);
	}
	{
		uint64_t ns_per_call = bulk_ns_per_call (
		    [&] () -> uint64_t {
			    auto t = std::chrono::system_clock::now ();
			    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds> (
			               t.time_since_epoch ())
			        .count ();
		    },
		    bulk_iters);
		double cyc = (double)ns_per_call * (double)cycles_per_sec / 1e9;
		std::printf (
		    "std::chrono::system_clock              [%7luns res]  %6.1f cycles  %5.1f ns/call\n",
		    (unsigned long)chrono_res_ns_system (),
		    cyc,
		    (double)ns_per_call);
	}

	std::puts ("-----\t-----\t-----");

	return 0;
}
