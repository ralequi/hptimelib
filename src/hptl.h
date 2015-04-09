#ifndef __HPTLIB__H__
#define __HPTLIB__H__

#include <inttypes.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

//Uncomment this line to enter in a debug mode
//#define __HPTL__DEBUGMODE__

//Type used to store timing in hptlib
typedef uint64_t hptl_t;

//Configuration structure
typedef struct
{
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

/**
 * Module initialization
 * @param conf configuration. If NULL, default configuration is used.
 * @return 0 if correct, -1 if there is an error with configuration.
 **/
int hptl_init(hptl_config * conf);

/**
 * Clock Syncronization with RTSC
 **/
void hptl_sync(void);

/**
 * Gets current time
 **/
hptl_t hptl_get(void);

/**
 * Return the resolution in terms of ns
 **/
uint64_t hptl_getres();

/**
 * Converts from HPTLib format to timespec format
 **/
struct timespec hptl_timespec(hptl_t hptltime);

/**
 * Converts from HPTLib format to ns from 01 Jan 1970
 **/
uint64_t hptl_ntimestamp(hptl_t hptltime);

#endif
