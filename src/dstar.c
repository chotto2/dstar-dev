/**
 * @file dstar.c
 * @brief Plot of divisors using the Sieve of Eratosthenes.
 * @author N.Arai
 * @date 2026-04-13
 *
 * This program plots the divisors of integers from 0 to n using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
 *
 * @note v1.4.0 (2026-04-13): Up to 10,000,000
 *       1. Improvement of memory usage efficiency.(approx. 0.8GB)
 *       2. Built-in benchmark measurement.
 *       3. Setting the upper limit of an integer by the first argument.
 *       4. Stop using uint64_t and change to uint32_t.
 *       
 * @note v1.3.1 (2026-04-06): Bug fix
 *       1. Fixed the array size for storing divisors (304 -> 344)
 *          - Note: d(2882880)=336 is the maximum within the range 0-3,000,000,
 *                  so the array size is set to 344. (approx. 4.2GB)
 *
 * @note v1.3.0 (2026-04-05): Up to 3,000,000
 *       1. Extended the upper limit of integers from 2,000,000 to 3,000,000
 *       2. Expanded the memory that holds divisors
 *          - Note: d(2948400)=300 is the maximum within the range 0-3,000,000,
 *                  so the array size is set to 304. (approx. 3.6GB)
 *       
 * @note v1.2.0 (2026-04-04): Up to 2,000,000
 *       1. Extended the upper limit of integers from 1,000,000 to 2,000,000
 *       2. Expanded the memory that holds divisors
 *          - Note: d(1441440)=288 is the maximum within the range 0–2,000,000, so the array size is set to 296. (approx. 2.4GB)
 *          - Note: Change to dynamic memory(calloc)
 *       
 * @note v1.1.0 (2026-03-29): Up to 1,000,000
 *       1. Extended the upper limit of integers from 1024 to 1,000,000
 *       2. Removed original bit operations
 *       3. Virtualized the existence of all divisors at x=0
 *       4. Improved memory efficiency by storing divisor values in an array
 *          - Note: d(720720)=240 is the maximum within the range 0–1,000,000, so the array size is set to 256. (approx. 1GB)
 *          - Note: Stored in ascending order, with NULL(0) as the terminator
 *
 * @note v1.0.0 (2026-01-29): Add dstar command
 *       1. Plots the divisors of integers from 0 to 1024
 *       2. Data retention by original bit operation
 *       3. Specifying the starting position is possible
 *
 * @note Do not display if the number of divisors exceeds 128.
 * @see Related paper on arXiv (to be published)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <ctype.h>

#define N_MAX n_max
#define M_MAX (N_MAX)
#define DSP_MAX (128)

#define ERR_OK		(0)
#define ERR_ARGSTYPE	(-1)
#define ERR_DIVSALOC	(-2)
#define ERR_POOLALOC	(-3)

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

typedef struct {
	uint32_t pool_ofs;
	uint32_t pool_cnt;
	uint32_t cnt;
} DIVS;


static uint32_t n_max = 1000000;
static int benchmark_mode = 0;
static struct rusage r_start, r_end;
static struct timeval wall_start, wall_end;

static int check_arg(int argc, char *argv[]);
static DIVS *calloc_divs(void);
static uint32_t set_pool_info(DIVS *divs);
static uint32_t *calloc_pool(uint32_t pool_num);
static void print_divisor_stars(DIVS *divs, uint32_t *divs_pool);

static int is_digits(const char *s) {
    if (s == NULL || *s == '\0') return 0;
    for (; *s; s++) {
        if (!isdigit((unsigned char)*s)) return 0;
    }
    return 1;
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[])
{
	DIVS     *divs;
	uint32_t *divs_pool;
	uint32_t pool_num;
	uint32_t m;
	uint32_t n;
	int ret;

	ret = check_arg(argc, argv);
	if (ret != ERR_OK) {
		;
	}
	else {
		divs = calloc_divs();
		if (divs == NULL) {
			ret = ERR_DIVSALOC;
		}
		else {
			pool_num = set_pool_info(divs);

			divs_pool = calloc_pool(pool_num);
			if (divs_pool == NULL) {
				ret = ERR_POOLALOC;
			}
			else {
				get_start_time();

				/*--- Sieve of Eratosthenes ---*/
				for (m = 1; m <= M_MAX; m++) {
					for (n = m; n <= N_MAX; n += m) {
						divs_pool[divs[n].pool_ofs+divs[n].cnt] = m;
						divs[n].cnt++;
					}
				}

				get_end_time();

				/*--- print results ---*/
				if (benchmark_mode) {
					print_benchmark();
				}
				else {
					print_divisor_stars(divs, divs_pool);
				}

				free(divs_pool);
				ret = ERR_OK;
			}

			free(divs);
		}
	}

	return ret;
}

static int check_arg(int argc, char *argv[])
{
	int ret = ERR_OK;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--benchmark") == 0) {
			benchmark_mode = 1;
		}
		else {
			if (i == 1) {
				if (is_digits(argv[i])) {
					n_max = atoll(argv[i]);
				}
				else {
					printf("USAGE: dstar [<n_max>] [--benchmark]\n");
					return ERR_ARGSTYPE;
				}
			}
		}
	}

	return ret;
}

static DIVS *calloc_divs()
{
	DIVS *p;

	p = calloc(N_MAX+1, sizeof(DIVS));
	if (p == NULL) {
		printf("ERR: divs(0) = calloc(%u, %ld)\n", N_MAX+1, sizeof(DIVS));
	}
//	printf("sizeof(DIVS) = %ld\n", sizeof(DIVS));
//	printf("sizeof(DIVS)*(N_MAX+1) = %ld\n", sizeof(DIVS)*(N_MAX+1));

	return p;
}


static uint32_t set_pool_info(DIVS *divs)
{
	uint32_t m;
	uint32_t n;
	uint32_t ofs;

	/*--- set pool_cnt ---*/
	for (m = 1; m <= M_MAX; m++) {
		for (n = m; n <= N_MAX; n += m) {
			divs[n].pool_cnt++;
		}
	}

	/*--- set pool_ofs ---*/
	ofs = 0;
	for (n = 1; n <= N_MAX; n++) {
		divs[n].pool_ofs = ofs;
		ofs += divs[n].pool_cnt;
		ofs++;	// for NULL padding.
	}

	return ofs;
}

static uint32_t *calloc_pool(uint32_t num)
{
	uint32_t *p;

	p = calloc(num, sizeof(uint32_t));
	if (p == NULL) {
		printf("ERR: divs_pool(0) = calloc(%u, %ld)\n", num, sizeof(uint32_t));
	}
//	printf("num = %u\n", num);
//	printf("num*sizeof(uint32_t) = %ld\n", num*sizeof(uint32_t));

	return p;
}

static void print_divisor_stars(DIVS *divs, uint32_t *divs_pool)
{
	uint32_t m;
	uint32_t n;
	uint32_t pre;
	uint32_t ofs;
	
	printf("      n:   d(n):divisors2(n, %u)\n", DSP_MAX);
	printf("%7u:%7u:", 0, N_MAX);
	for (m = 1; m <= DSP_MAX; m++) {
		printf("*");
	}
	printf("...\n");
	for (n = 1; n <= N_MAX; n++) {
		printf("%7u:%7u:", n, divs[n].cnt);
		pre = 0;
		for (ofs = 0; ofs < divs[n].pool_cnt; ofs++) {
			m = divs_pool[divs[n].pool_ofs+ofs];
			if (m == 0) break;
			if (m > DSP_MAX) continue;
			if (pre) {
				for (int i = 0; i < (m - pre - 1); i++) printf(" ");
			}
			printf("*");
			pre = m;
		}
		printf("\n");
	}

	return;
}

