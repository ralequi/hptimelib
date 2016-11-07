#include <hptl.h>
#include <time.h>

#define TESTREPEAT   1000L
#define TESTTIME 10000000L

struct timespec diff(struct timespec start, struct timespec end, char *sign);

extern uint64_t __hptl_hz;

int main(/*int argc, char **argv*/)
{
	hptl_config confhptl;
	confhptl.precision = 9;
	confhptl.clockspeed = 0;

	hptl_init(&confhptl);

	unsigned long i = 0, j = 0;
	uint64_t tmp = 0;
	char sign = '+';

	struct timespec cmtime, rt, df;

	printf("[Precised] Deviation Test started...[%lu]\n", hptl_get());

	for (j = 0; j < TESTREPEAT; j++) {
		printf("Test #%lu of %lu...", j + 1, TESTREPEAT);
		fflush(stdin);

		for (i = 0; i < TESTTIME; i++) {
			tmp = hptl_get();
		}

		if (clock_gettime(CLOCK_REALTIME, &cmtime) == -1) {
			perror("clock gettime");
			return -1;
		}

		rt = hptl_timespec(tmp);
		df = diff(cmtime, rt, &sign);
		printf(" Deviation of %c %lu s, %3lu ms, %3lu us, %3lu ns from clock_gettime(CLOCK_REALTIME).\n",
			   sign,
			   df.tv_sec,
			   (df.tv_nsec / 1000000000L) % 1000L,
			   (df.tv_nsec / 1000L) % 1000L,
			   df.tv_nsec % 1000L);

	}

	return 0;
}


struct timespec diff(struct timespec start, struct timespec end, char *sign) {
	struct timespec temp;

	if (start.tv_sec > end.tv_sec) {
		temp = end;
		end = start;
		start = temp;

		if (sign != NULL) {
			*sign = '-';
		}
	} else if (start.tv_sec == end.tv_sec && start.tv_nsec > end.tv_nsec) {
		temp = end;
		end = start;
		start = temp;

		if (sign != NULL) {
			*sign = '-';
		}
	} else if (sign != NULL) {
		*sign = '+';
	}

	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000ull + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}
