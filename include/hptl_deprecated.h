/************************************************
 * This API is deprecated. It remains only for compatibility
 ************************************************/
/**
  Language: C

  License: MIT License
  (c) HPCN 2014-2017
  (c) Naudit HPCN S.L. 2018-2021

  Author: Rafael Leira
  E-Mail: rafael.leira@naudit.es

  Description: Old header file for hptl

  Thread Safe: No

  Platform Dependencies: Linux-like and Intel Processor
*/
#ifndef __HPTLIB_DEPRECATED__H__
#define __HPTLIB_DEPRECATED__H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Module initialization
 * @param conf configuration. If NULL, default configuration is used.
 * @return 0 if correct, -1 if there is an error with configuration.
 **/
int hptl_init (hptl_config *conf);

/**
 * Clock Syncronization with RTSC
 **/
void hptl_sync (void);

/**
 * HZ calibration
 * @param diffTime the time between executions, example if hptl_get takes 17ns
 *and clockgettime 22ns, 5 (22-17) should be used
 * @return the hz modified
 **/
int hptl_calibrateHz (int diffTime);

/**
 * Gets current time
 **/
hptl_t hptl_get (void);

/**
 * Gets current time. The return would be in the format of the fatest function available in the
 *system. Acuracy nor precision are not guaranteed
 **/
hptl_t hptl_fget (void);

/**
 * Return the resolution in terms of ns
 **/
uint64_t hptl_getres (void);

/**
 * Convert a certain number of nanoseconds into clock-cycles
 * @param ns the ns to convert into clock-cycles
 **/
uint64_t hptl_n2c (uint64_t ns);

/**
 * Wait certain ns actively.
 * hptl should be initialized first.
 **/
void hptl_waitns (uint64_t ns);

/** UTILS **/

/**
 * Converts from HPTLib format to timespec format
 **/
struct timespec hptl_timespec (hptl_t hptltime);

/**
 * Converts from HPTLib format to timeval format
 **/
struct timeval hptl_timeval (hptl_t hptltime);

/**
 * Converts from HPTLib format to ns from 01 Jan 1970
 **/
uint64_t hptl_ntimestamp (hptl_t hptltime);

#ifdef __cplusplus
}
#endif

#endif
