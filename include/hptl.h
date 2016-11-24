/**
  Language: C

  License: MIT License
  (c) HPCN 2014-2017

  Author: Rafael Leira
  E-Mail: rafael.leira@uam.es

  Description: High Performance Timing Library, that uses RTC counter.

  Thread Safe: Yes

  Platform Dependencies: Linux-like and Intel Processor
*/
#ifndef __HPTLIB__H__
#define __HPTLIB__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

// Config file
#include "hptl_config.h"

// check for Errors
#ifndef HPTL_TSC
#error constant_tsc is not present
#endif

#ifdef HPTL_ONLYLINUXAPI
#ifndef HPTL_CLOCKREALTIME
#error HPTL_ONLYLINUXAPI does not work if HPTL_CLOCKREALTIME does not work (rt library is not present).
#endif
#endif

// Type used to store timing in hptlib
typedef uint64_t hptl_t;

// Configuration structure
typedef struct {
	/* Sets the precision of the hptl.
	 * Some examples of possible values are:
	 * 9 sets de precission to ns
	 * 8 sets de precission to tens of ns
	 * 7 sets de precission to hundreds of ns
	 * 6 sets de precission to us
	 * 3 sets de precission to ms
	 * 0 sets de precission to s
	 */
	uint8_t precision;
	/* CPU clock speed in Hz.
	 * if unknown, set the value to 0 and will be autodetected
	 */
	uint64_t clockspeed;
} hptl_config;

// Configuration structure
typedef struct {
	uint64_t __hptl_time;
	uint64_t __hptl_cicles;
	uint64_t __hptl_hz;
	uint64_t __hptl_precision;
} hptl_clock;

#include "hptl_deprecated.h"

/**
 * Module initialization
 * @param clk the hptl clk structure.
 * @param conf configuration. If NULL, default configuration is used.
 * @return 0 if correct, -1 if there is an error with configuration.
 **/
int hptl_initclk (hptl_clock *clk, hptl_config *conf);

/**
 * Clock Syncronization with RTSC
 * @param clk the hptl clk structure.
 **/
void hptl_syncclk (hptl_clock *clk);

/**
 * HZ calibration
 * @param clk the hptl clk structure.
 * @param diffTime the time between executions, example if hptl_get takes 17ns
 *and clockgettime 22ns, 5 (22-17) should be used
 * @return the hz modified
 **/
int hptl_calibrate (hptl_clock *clk, int diffTime);

/**
 * Gets current time
 * @param clk the hptl clk structure.
 **/
hptl_t hptl_getTime (hptl_clock *clk);

/**
 * Convert a certain number of nanoseconds into clock-cycles
 * @param clk the hptl clk structure.
 * @param ns the ns to convert into clock-cycles
 **/
uint64_t hptl_ns2cycles (hptl_clock *clk, uint64_t ns);

/**
 * Return the resolution in terms of ns
 * @param clk the hptl clk structure.
 **/
uint64_t hptl_getclkres (hptl_clock *clk);

/**
 * Wait certain ns actively.
 * @param clk the hptl clk structure.
 **/
void hptl_wait (hptl_clock *clk, uint64_t ns);

/**
 * Wait certain cycles actively.
 * @param clk the hptl clk structure.
 **/
void hptl_wait_cycles (uint64_t cycles);

/** UTILS **/

/**
 * Converts from HPTLib format to timespec format
 **/
struct timespec hptl_clktimespec (hptl_clock *clk, hptl_t hptltime);

/**
 * Converts from HPTLib format to timeval format
 **/
struct timeval hptl_clktimeval (hptl_clock *clk, hptl_t hptltime);

/**
 * Converts from HPTLib format to ns from 01 Jan 1970
 **/
uint64_t hptl_clkntimestamp (hptl_clock *clk, hptl_t hptltime);

#ifdef __cplusplus
}
#endif

#endif
