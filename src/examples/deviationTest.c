#include "../realtime.h"
#include <time.h>

#define TESTREPEAT   1000L
#define TESTTIME 10000000L

struct timespec diff(struct timespec start, struct timespec end);

int main(int argc, char**argv)
{
	realtime_init();

	unsigned long i=0,j=0;
	uint64_t tmp = 0;

	struct timespec cmtime,rt,df;

	printf("Deviation Test started...[%lu]\n",realtime_get());

	for (j=0;j<TESTREPEAT;j++)
	{
		printf("Test #%lu of %lu...",j+1,TESTREPEAT); fflush(stdin);

		for(i=0;i<TESTTIME;i++)
		{
			tmp = realtime_get();
		}

		if( clock_gettime( CLOCK_REALTIME, &cmtime) == -1 ) {
			perror( "clock gettime" );
			return -1;
		}

		rt = realtime_timespec(tmp);
		df = diff(cmtime,rt);
		printf(" Deviation of %lu and %lu ns from clock_gettime(CLOCK_REALTIME)\n",df.tv_sec,df.tv_nsec);

	}

	return 0;
}


struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;

	if(start.tv_sec > end.tv_sec)
	{
		temp=end;
		end=start;
		start=temp;
	}
	else if (start.tv_sec == end.tv_sec && start.tv_nsec > end.tv_nsec)
        {
                temp=end;
                end=start;
                start=temp;
        }

	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000ull+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}
