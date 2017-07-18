#include <hptl.h>
#include <sys/time.h>
#include <time.h>

#define TESTREPEAT 1000000000L

extern uint64_t __hptl_hz;

#define RED "\x1b[31m"
#define RST "\x1b[0m"

int main (/*int argc, char **argv*/) {
	hptl_config confhptl;
	confhptl.precision  = 9;
	confhptl.clockspeed = 0;

	hptl_init (&confhptl);

	unsigned long i = 0;
	struct timespec df;

	printf ("Integrity Test started [resolution = %lu]...[%lu]\n", hptl_getres (), hptl_get ());

	hptl_t start, end;
	hptl_t last, current;

	uint64_t sameValues, backtimes;

	printf ("Testing HPTL for %ld repetitions...", TESTREPEAT);
	fflush (stdout);

	last = current = sameValues = backtimes = 0;

	start = hptl_get ();

	for (i = 0; i < TESTREPEAT; i++) {
		current = hptl_get ();
		if (current == last)
			sameValues++;
		if (last > current)
			backtimes++;
		last = current;
	}

	end = hptl_get ();

	df = hptl_timespec (end - start);

	printf ("Test took %lu s and has "RED"%lu"RST" repetitions and "RED"%lu"RST" backtimes.\n",
	        df.tv_sec,
	        sameValues,
	        backtimes);

	/***********************************************************************************/
	struct timespec lastts, currentts;

	printf ("Testing clock_gettime/CLOCK_REALTIME for %ld repetitions...", TESTREPEAT);
	fflush (stdout);

	sameValues = backtimes = 0;
	currentts.tv_sec       = 0;
	currentts.tv_nsec      = 0;
	lastts                 = currentts;

	start = hptl_get ();

	for (i = 0; i < TESTREPEAT; i++) {
		if (clock_gettime (CLOCK_REALTIME, &currentts) == -1) {
			perror ("clock gettime");
			return -1;
		}

		if (currentts.tv_sec == lastts.tv_sec && currentts.tv_nsec == lastts.tv_nsec)
			sameValues++;
		if ((currentts.tv_sec == lastts.tv_sec && currentts.tv_nsec < lastts.tv_nsec) ||
		    currentts.tv_sec < lastts.tv_sec)
			backtimes++;
		lastts = currentts;
	}

	end = hptl_get ();

	df = hptl_timespec (end - start);

	printf ("Test took %lu s and has "RED"%lu"RST" repetitions and "RED"%lu"RST" backtimes.\n",
	        df.tv_sec,
	        sameValues,
	        backtimes);

	/***********************************************************************************/
	printf ("Testing clock_gettime/CLOCK_MONOTONIC for %ld repetitions...", TESTREPEAT);
	fflush (stdout);

	sameValues = backtimes = 0;
	currentts.tv_sec       = 0;
	currentts.tv_nsec      = 0;
	lastts                 = currentts;

	start = hptl_get ();

	for (i = 0; i < TESTREPEAT; i++) {
		if (clock_gettime (CLOCK_MONOTONIC, &currentts) == -1) {
			perror ("clock gettime");
			return -1;
		}

		if (currentts.tv_sec == lastts.tv_sec && currentts.tv_nsec == lastts.tv_nsec)
			sameValues++;
		if ((currentts.tv_sec == lastts.tv_sec && currentts.tv_nsec < lastts.tv_nsec) ||
		    currentts.tv_sec < lastts.tv_sec)
			backtimes++;
		lastts = currentts;
	}

	end = hptl_get ();

	df = hptl_timespec (end - start);

	printf ("Test took %lu s and has "RED"%lu"RST" repetitions and "RED"%lu"RST" backtimes.\n",
	        df.tv_sec,
	        sameValues,
	        backtimes);

	/***********************************************************************************/
	struct timeval lasttv, currenttv;

	printf ("Testing gettimeofday for %ld repetitions...", TESTREPEAT);
	fflush (stdout);

	sameValues = backtimes = 0;
	currenttv.tv_sec       = 0;
	currenttv.tv_usec      = 0;
	lasttv                 = currenttv;

	start = hptl_get ();

	for (i = 0; i < TESTREPEAT; i++) {
		if (gettimeofday (&currenttv, NULL) == -1) {
			perror ("gettimeofday");
			return -1;
		}

		if (currenttv.tv_sec == lasttv.tv_sec && currenttv.tv_usec == lasttv.tv_usec)
			sameValues++;
		if ((currenttv.tv_sec == lasttv.tv_sec && currenttv.tv_usec < lasttv.tv_usec) ||
		    currenttv.tv_sec < lasttv.tv_sec)
			backtimes++;
		lasttv = currenttv;
	}

	end = hptl_get ();

	df = hptl_timespec (end - start);

	printf ("Test took %lu s and has "RED"%lu"RST" repetitions and "RED"%lu"RST" backtimes.\n",
	        df.tv_sec,
	        sameValues,
	        backtimes);

	return 0;
}
