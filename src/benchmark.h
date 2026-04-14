
#ifndef BENCHMARK_H
#define BENCHMARK_H

static struct rusage r_start, r_end;
static struct timeval wall_start, wall_end;

#define get_start_time() {			\
	gettimeofday(&wall_start, NULL);	\
	getrusage(RUSAGE_SELF, &r_start);	\
}

#define get_end_time() {			\
	gettimeofday(&wall_end, NULL);		\
	getrusage(RUSAGE_SELF, &r_end);		\
}

#define print_benchmark() {								\
	double wall = (wall_end.tv_sec - wall_start.tv_sec)				\
		    + (wall_end.tv_usec - wall_start.tv_usec) / 1e6;			\
	double user = (r_end.ru_utime.tv_sec  - r_start.ru_utime.tv_sec)		\
		    + (r_end.ru_utime.tv_usec - r_start.ru_utime.tv_usec) / 1e6;	\
	double sys  = (r_end.ru_stime.tv_sec  - r_start.ru_stime.tv_sec)		\
		    + (r_end.ru_stime.tv_usec - r_start.ru_stime.tv_usec) / 1e6;	\
	printf("real %.3fs user %.3fs  sys %.3fs\n", wall, user, sys);			\
}

#endif	// for BENCHMARK_H
