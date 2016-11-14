#include "hptl.h"

/******************** VARIABLES ********************/
static hptl_clock globalClk;

/********************* FUNCTIONS *********************/

int hptl_init (hptl_config *conf) { return hptl_initclk (&globalClk, conf); }

void hptl_sync (void) { hptl_syncclk (&globalClk); }

int hptl_calibrateHz (int diffTime) { return hptl_calibrate (&globalClk, diffTime); }

hptl_t hptl_get (void) { return hptl_getTime (&globalClk); }

uint64_t hptl_getres (void) { return hptl_getclkres (&globalClk); }

void hptl_waitns (uint64_t ns) { hptl_wait (ns); }

struct timespec hptl_timespec (hptl_t hptltime) {
	return hptl_clktimespec (&globalClk, hptltime);
}

struct timeval hptl_timeval (hptl_t hptltime) {
	return hptl_clktimeval (&globalClk, hptltime);
}

uint64_t hptl_ntimestamp (hptl_t hptltime) { return hptl_clkntimestamp (&globalClk, hptltime); }
