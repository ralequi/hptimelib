#include "hptl.h"

// Old format convertion
#define __hptl_time (clk->__hptl_time)
#define __hptl_cicles (clk->__hptl_cicles)
#define __hptl_hz (clk->__hptl_hz)
#define __hptl_precision (clk->__hptl_precision)

#define PRECCISION (__hptl_precision)

/********************* MACROS *********************/
#define overflowflag(isOverflow)           \
	do {                                   \
		asm volatile(                      \
		    "pushf ;"                      \
		    "pop %%rax"                    \
		    : "=a"(isOverflow));           \
		isOverflow = (isOverflow & 0x800); \
	} while (0)

/*************** STRUCTURES & UNIONS ***************/
typedef union {
	uint64_t tsc_64;
	struct {
		uint32_t lo_32;
		uint32_t hi_32;
	};
} _hptlru;  // hptl rdtsc union.

/*************** PRIVATE FUNCTIONS ***************/
struct timespec hptl_ts_diff (struct timespec start, struct timespec end, char *sign);

/*************** iDPDK FUNCTIONS ***************/
#ifndef HPTL_ONLYLINUXAPI
// Do not define satatic structures
static inline uint64_t hptl_rdtsc (void) {
	_hptlru tsc;

	asm volatile("rdtsc" : "=a"(tsc.lo_32), "=d"(tsc.hi_32));

	return tsc.tsc_64;
}

static int set_tsc_freq_from_clock (hptl_clock *clk) {
#ifdef CLOCK_MONOTONIC_RAW
#define NS_PER_SEC 1E9
	uint64_t ns, end, start;
#ifdef HPTL_DEBUG
	printf ("[HPTLib] Using CLOCK_MONOTONIC_RAW to obtain CPU Hz...\n");
#endif

	struct timespec sleeptime = {.tv_sec = 0, .tv_nsec = 500000000}; /* 1/2 second */

	struct timespec t_start, t_end;

	if (clock_gettime (CLOCK_MONOTONIC_RAW, &t_start) == 0) {
		start = hptl_rdtsc ();
		nanosleep (&sleeptime, NULL);
		clock_gettime (CLOCK_MONOTONIC_RAW, &t_end);
		end = hptl_rdtsc ();

		ns = ((t_end.tv_sec - t_start.tv_sec) * NS_PER_SEC);
		ns += (t_end.tv_nsec - t_start.tv_nsec);

		double secs = (double)ns / NS_PER_SEC;
		__hptl_hz   = (uint64_t) ((end - start) / secs);
		return 0;
	}

#endif
	return -1;
}

static void set_tsc_freq_linux (hptl_clock *clk) {
	char tmp[100];  //"/sys/devices/system/cpu/cpuXXX/cpufreq/cpuinfo_cur_freq";
	volatile int i;
	int status;
	unsigned cpu;
	FILE *f;

#ifdef HPTL_DEBUG
	printf ("[HPTLib] Using Linux to obtain CPU Hz...\n");
#endif

	status = syscall (SYS_getcpu, &cpu, NULL, NULL);

	if (status == -1) {
#ifdef HPTL_DEBUG
		printf ("[HPTLib] ERROR: HPTLib.set_tsc_freq_linux.syscall\n");
#endif
		exit (-1);
	}

	sprintf (tmp, "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_cur_freq", cpu);

#ifdef HPTL_DEBUG
	printf ("[HPTLib] Assuming HPTLib is going to run on %d...\n", cpu);
#endif

	f = fopen (tmp, "r");

	if (f == NULL) {
		perror ("HPTLib.set_tsc_freq_linux.fopen");
		exit (-1);
	}

	for (i = 0; i < 15000000; i++)
		;  // warm the CPU

	if (fgets (tmp, sizeof (tmp), f) == NULL) {
		perror ("HPTLib.set_tsc_freq_linux.fgets");
		exit (-1);
	}

	__hptl_hz = atol (tmp) * 1000ul;

	fclose (f);
}

/*
 * This function measures the TSC frequency. It uses a variety of approaches.
 *
 * 1. If kernel provides CLOCK_MONOTONIC_RAW we use that to tune the TSC value
 * 2. If kernel does not provide that, and we have HPET support, tune using HPET
 * 3. Lastly, if neither of the above can be used, just sleep for 1 second and
 * tune off that, printing a warning about inaccuracy of timing
 */
static void set_tsc_freq (hptl_clock *clk) {
	if (set_tsc_freq_from_clock (clk) < 0) {
		set_tsc_freq_linux (clk);
	}
}
#endif

/********************* FUNCTIONS *********************/

int hptl_initclk (hptl_clock *clk, hptl_config *conf) {
	hptl_config config;
	int i, k = 1;

#ifdef HPTL_DEBUG
	printf ("[HPTL] INFO: Starting HPTL %s\n", hptl_VERSION);
#endif

	// load config
	if (conf == NULL) {
		config.precision  = 7;
		config.clockspeed = 0;

	} else {
		config = *conf;
	}

	if (config.precision > 9) {
#ifdef HPTL_DEBUG
		printf ("[HPTLib] Error: precision %u>9\n", config.precision);
#endif
		return -1;
	}

	// config precision
	for (i = 0; i < config.precision; i++) {
		k *= 10;
	}

	PRECCISION = k;

#ifdef HPTL_ONLYLINUXAPI
// do nothing more specific
#else
	// Specific and advanced options

	// load clockspeed
	if (config.clockspeed == 0) {
		set_tsc_freq (clk);

	} else {
		__hptl_hz = config.clockspeed;
	}

	hptl_sync ();

	if (config.clockspeed == 0) {
		hptl_calibrateHz (0);
	}

#ifdef HPTL_DEBUG
	printf (
	    "[HPTLib] Started : Hz:%lu cicles:%lu tof:%lu\n", __hptl_hz, __hptl_cicles, __hptl_time);
#endif
#endif

	return 0;  // always return 0
}

void hptl_syncclk (hptl_clock *clk) {
#ifdef HPTL_ONLYLINUXAPI
// do nothing
#else
	struct timespec tmp;

	if (clock_gettime (CLOCK_REALTIME, &tmp) != 0) {
		printf ("[HPTLib] WARN: Clock_gettime ERROR!\n");
	}

	__hptl_cicles = hptl_rdtsc ();
	__hptl_time = tmp.tv_sec * PRECCISION + tmp.tv_nsec / (1000000000ull / PRECCISION);

#ifdef HPTL_DEBUG
	printf ("[HPTLib] Sync: cicles:%lu tof:%lu\n", __hptl_cicles, __hptl_time);
#endif
#endif
}

/*
 * HZ calibration
 * @param oldTime time obtained by hptl_get()
 * @param newTime the newer time obtained by system precision-function
 * @param diffTime the time between executions, example if hptl_get takes 17ns
 * and clockgettime 22ns, 5 (22-17) should be used
 * @return the hz modified
 */
int hptl_calibrate (hptl_clock *clk, int diffTime) {
#ifdef HPTL_ONLYLINUXAPI
	// do nothing
	(void)diffTime;
	return 0;
#else
	struct timespec newTime;

	// get the hptltime
	unsigned long long tmp;
	volatile unsigned long long Oflag;

	_hptlru tsc;

	hptl_waitns (750000000);

	asm volatile("rdtsc" : "=a"(tsc.lo_32), "=d"(tsc.hi_32));

	tmp = ((tsc.tsc_64 - __hptl_cicles) * PRECCISION);

	overflowflag (Oflag);

	if (Oflag) {
		hptl_sync ();
		return hptl_calibrateHz (diffTime);
	}

	// calibrates the time provided by diffTime
	clock_gettime (CLOCK_REALTIME, &newTime);
	newTime.tv_nsec += diffTime;
	int hzCalibrated = 0;

	unsigned long long newhptl;
	struct timespec error, errorPrima;

	errorPrima = hptl_ts_diff (hptl_timespec ((tmp / __hptl_hz) + __hptl_time), newTime, NULL);

	do {
		error = errorPrima;
		hzCalibrated++;

		newhptl    = (tmp / (__hptl_hz + hzCalibrated)) + __hptl_time;
		errorPrima = hptl_ts_diff (hptl_timespec (newhptl), newTime, NULL);

		/*printf("\n Was %c %lu s, %3lu ms, %3lu us, %3lu ns ; now %c
		   %lu s, %3lu ms, %3lu us, %3lu ns\n",
		           sign,
		           error.tv_sec,
		           (error.tv_nsec / 1000000000L) % 1000L,
		           (error.tv_nsec / 1000L) % 1000L,
		           error.tv_nsec % 1000L,
		           signPrima,
		           errorPrima.tv_sec,
		           (errorPrima.tv_nsec / 1000000000L) % 1000L,
		           (errorPrima.tv_nsec / 1000L) % 1000L,
		           errorPrima.tv_nsec % 1000L
		            );*/
	} while (errorPrima.tv_nsec <= error.tv_nsec &&
	         (uint64_t)errorPrima.tv_nsec > (10000000000ull / PRECCISION));

	if (hzCalibrated == 1) {  // Execute only, if previous execution does not
		                      // calibrated (but descalibrated).
		do {
			error = errorPrima;
			hzCalibrated--;

			newhptl    = (tmp / (__hptl_hz + hzCalibrated)) + __hptl_time;
			errorPrima = hptl_ts_diff (hptl_timespec (newhptl), newTime, NULL);
		} while (errorPrima.tv_nsec <= error.tv_nsec &&
		         (uint64_t)errorPrima.tv_nsec > (10000000000ull / PRECCISION));
	}

	__hptl_hz += hzCalibrated;
	return hzCalibrated;
#endif
}

hptl_t hptl_getTime (hptl_clock *clk) {
#ifdef HPTL_ONLYLINUXAPI
	// Wraper mode
	struct timespec cmtime;
	clock_gettime (CLOCK_REALTIME, &cmtime);
	hptl_t ret = cmtime.tv_nsec / (1000000000ull / PRECCISION) + cmtime.tv_sec * PRECCISION;
	return ret;
#else
	// Advanced high-eficiency mode
	unsigned long long tmp;
	volatile unsigned long long Oflag;

	_hptlru tsc;

	asm volatile("rdtsc" : "=a"(tsc.lo_32), "=d"(tsc.hi_32));

	tmp = ((tsc.tsc_64 - __hptl_cicles) * PRECCISION);

	overflowflag (Oflag);

	if (Oflag) {
		hptl_sync ();
		return hptl_get ();
	}

	return (tmp / __hptl_hz) + __hptl_time;
#endif
}

/**
 * Return the resolution in terms of ns
 **/
uint64_t hptl_getclkres (hptl_clock *clk) {
	return 1000000000ull / PRECCISION;
}

/**
 * Convert a certain number of nanoseconds into clock-cycles
 * @param clk the hptl clk structure.
 * @param ns the ns to convert into clock-cycles
 **/
uint64_t hptl_ns2cycles (hptl_clock *clk, uint64_t ns) {
	return (ns * 1000000000ull) / __hptl_hz;
}

/**
 * Wait certain ns actively
 **/
void hptl_wait (uint64_t ns) {
#ifdef HPTL_ONLYLINUXAPI
	struct timespec sleeptime;
	sleeptime.tv_sec  = ns / 1000000000ull;
	sleeptime.tv_nsec = ns % 1000000000ull;
	nanosleep (&sleeptime, NULL);
#else
	hptl_t start, end;

	// start = hptl_rdtsc();
	start = hptl_get ();

	// float cycles = ((float)ns)*(((float)__hptl_hz)/1000000000.);
	// end = start + cycles;
	end = start + ns;

	do {
		// start = hptl_rdtsc();
		start = hptl_get ();
	} while (start < end);
#endif
}

/**
 * Wait certain cycles actively.
 * @param clk the hptl clk structure.
 **/
void hptl_wait_cycles (uint64_t cycles) {
	uint64_t tscreg = hptl_rdtsc ();
	uint64_t dest = tscreg + cycles;
	while (hptl_rdtsc () < dest)
		;
}

/**
 * Converts from realtime format to timespect format
 **/
struct timespec hptl_clktimespec (hptl_clock *clk, hptl_t u64) {
	struct timespec tmp;

	tmp.tv_sec  = u64 / PRECCISION;
	tmp.tv_nsec = (u64 - (tmp.tv_sec * PRECCISION)) * (1000000000ull / PRECCISION);

	return tmp;
}

/**
 * Converts from realtime format to timeval format
 **/
struct timeval hptl_clktimeval (hptl_clock *clk, hptl_t u64) {
	struct timeval tmp;

	tmp.tv_sec  = u64 / PRECCISION;
	tmp.tv_usec = ((u64 - (tmp.tv_sec * PRECCISION)) * 1000000ull) / PRECCISION;

	return tmp;
}

/**
 * Converts from HPTLib format to ns from 01 Jan 1970
 **/
uint64_t hptl_clkntimestamp (hptl_clock *clk, hptl_t hptltime) {
	return hptltime * (1000000000ull / PRECCISION);
}

struct timespec hptl_ts_diff (struct timespec start, struct timespec end, char *sign) {
	struct timespec temp;

	if (start.tv_sec > end.tv_sec) {
		temp  = end;
		end   = start;
		start = temp;

		if (sign != NULL) {
			*sign = '-';
		}
	} else if (start.tv_sec == end.tv_sec && start.tv_nsec > end.tv_nsec) {
		temp  = end;
		end   = start;
		start = temp;

		if (sign != NULL) {
			*sign = '-';
		}
	} else if (sign != NULL) {
		*sign = '+';
	}

	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000ull + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec  = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}