#include <hptl.h>

struct timespec diff(struct timespec start, struct timespec end);

int main(/*int argc, char **argv*/)
{
	hptl_config confhptl;

	printf("hptlib example test...\n");

	//metric variables
	hptl_t start, end;
	hptl_t dif;

	confhptl.precision = 9;
	confhptl.clockspeed = 0;

	printf("Configuring hptlib with precision=%hhu and default clockspeed=%lu (0 means AUTO)\n",
		   confhptl.precision, confhptl.clockspeed);
	hptl_init(&confhptl);

	//getting resolution
	printf("Hptlib efectivly configured with precision of %lu 0s\n", hptl_getres());

	//Getting RAW time
	printf("RAW time : %lu\n", hptl_get());

	//Get Time in timeval format:
	struct timeval tmptimeval;

	tmptimeval = hptl_timeval(hptl_get());
	printf("Time in timeval structure, Seconds = %ld ; MicroSeconds = %ld\n",
		   tmptimeval.tv_sec, tmptimeval.tv_usec);

	tmptimeval = hptl_timeval(hptl_get());
	printf("Time in timeval structure, Seconds = %ld ; MicroSeconds = %ld\n",
		   tmptimeval.tv_sec, tmptimeval.tv_usec);


	//Get Time in timespec format:
	struct timespec tmptimespec;

	tmptimespec = hptl_timespec(hptl_get());
	printf("Time in timespec structure, Seconds = %ld ; NanoSeconds = %ld\n",
		   tmptimespec.tv_sec, tmptimespec.tv_nsec);

	tmptimespec = hptl_timespec(hptl_get());
	printf("Time in timespec structure, Seconds = %ld ; NanoSeconds = %ld\n",
		   tmptimespec.tv_sec, tmptimespec.tv_nsec);




	//Waiting Example

	printf("Waiting 3 Seconds...");
	fflush(stdout);

	start = hptl_get();
	hptl_waitns(3000000000L);
	end = hptl_get();

	dif = end - start;

	printf("Done! (Exactly waited %lu s, %lu ms, %lu us, %lu ns)\n"
		   , dif / 1000000000
		   , dif / 1000000    - (dif / 1000000000) * 1000
		   , dif / 1000       - (dif / 1000000) * 1000
		   , dif            - (dif / 1000) * 1000);

	fflush(stdout);

	return 0;
}
