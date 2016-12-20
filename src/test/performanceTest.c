#include <hptl.h>
#include <time.h>
#include <sys/time.h>

#define TESTREPEAT 3llu
#define TESTTIME 100000000llu

struct timespec diff (struct timespec start, struct timespec end);

int main (/*int argc, char **argv*/) {
	uint64_t i = 0, j = 0;
	hptl_config confhptl;

	// test variables
	volatile uint64_t tmp = 0;
	struct timespec cmtime, resolution;
	struct timeval gdtime;

	// metric variables
	hptl_t start, end;
	hptl_t dif;

	confhptl.precision  = 9;
	confhptl.clockspeed = 0;
	hptl_init (&confhptl);

	printf ("Performance Test started...[%lu]\n", hptl_get ());

	for (j = 0; j < TESTREPEAT; j++) {
		// HPCTimeLib Test
		printf ("HPCTimeLib #%lu [...]", j + 1);
		fflush (stdout);
		start = hptl_get ();

		for (i = 0; i < TESTTIME; i++) {
			tmp = hptl_get ();
		}

		end = hptl_get ();
		dif = end - start;
		printf (
		    "\rHPCTimeLib                           [%7ldns res]  #%lu [%02llu "
		    "ns/call] -- Finished at %lu\n",
		    hptl_getres (),
		    j + 1,
		    hptl_ntimestamp (dif) / TESTTIME,
		    tmp);

		// Clock_Realtime Test
		printf ("clock_gettime(CLOCK_REALTIME) #%lu [...]", j + 1);
		fflush (stdout);
		start = hptl_get ();

		for (i = 0; i < TESTTIME; i++) {
			clock_gettime (CLOCK_REALTIME, &cmtime);
		}

		end = hptl_get ();
		dif = end - start;
		clock_getres (CLOCK_REALTIME, &resolution);
		printf (
		    "\rclock_gettime(CLOCK_REALTIME)        [%7ldns res]  #%lu [%02llu "
		    "ns/call] -- Finished at %llu\n",
		    resolution.tv_nsec,
		    j + 1,
		    hptl_ntimestamp (dif) / TESTTIME,
		    cmtime.tv_sec * 10000000llu + cmtime.tv_nsec);

		// Clock_Realtime Test
		printf ("clock_gettime(CLOCK_REALTIME_COARSE) #%lu [...]", j + 1);
		fflush (stdout);
		start = hptl_get ();

		for (i = 0; i < TESTTIME; i++) {
			clock_gettime (CLOCK_REALTIME_COARSE, &cmtime);
		}

		end = hptl_get ();
		dif = end - start;
		clock_getres (CLOCK_REALTIME_COARSE, &resolution);
		printf (
		    "\rclock_gettime(CLOCK_REALTIME_COARSE) [%7ldns res]  #%lu [%02llu "
		    "ns/call] -- Finished at %llu\n",
		    resolution.tv_nsec,
		    j + 1,
		    hptl_ntimestamp (dif) / TESTTIME,
		    cmtime.tv_sec * 10000000llu + cmtime.tv_nsec);

		// GetTimeOfDay Test
		printf ("gettimeofday(NULL) #%lu [...]", j + 1);
		fflush (stdout);
		start = hptl_get ();

		for (i = 0; i < TESTTIME; i++) {
			gettimeofday (&gdtime, NULL);
		}

		end = hptl_get ();
		dif = end - start;
		printf (
		    "\rgettimeofday(NULL)                   [%7ldns res]  #%lu [%02llu "
		    "ns/call] -- Finished at %llu\n",
		    1000l,
		    j + 1,
		    hptl_ntimestamp (dif) / TESTTIME,
		    gdtime.tv_sec * 10000000llu + gdtime.tv_usec * 1000);

		puts ("-----\t-----\t-----");
		//		rt = realtime_timespec(tmp);
	}

	return 0;
}

struct timespec diff (struct timespec start, struct timespec end) {
	struct timespec temp;

	if (start.tv_sec > end.tv_sec) {
		temp  = end;
		end   = start;
		start = temp;
	} else if (start.tv_sec == end.tv_sec && start.tv_nsec > end.tv_nsec) {
		temp  = end;
		end   = start;
		start = temp;
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
