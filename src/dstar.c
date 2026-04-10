/**
 * @file dstar.c
 * @brief Plot of divisors using the Sieve of Eratosthenes.
 * @author N.Arai
 * @date 2026-04-08
 *
 * This program plots the divisors of integers from 0 to n using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
 *
 * @note v1.4.0 (2026-04-08): Up to 4,000,000
 *       1. Extended the upper limit of integers from 3,000,000 to 4,000,000
 *       2. Expanded the memory that holds divisors
 *          - Note: d(3603600)=360 is the maximum within the range 0-4,000,000,
 *                  so the array size is set to 360. (approx. 5.8GB)
 *       
 * @note v1.3.1 (2026-04-06): Bug fix
 *       1. Fixed the array size for storing divisors (304 -> 344)
 *          - Note: d(2882880)=336 is the maximum within the range 0-3,000,000,
 *                  so the array size is set to 344. (approx. 4.2GB)
 *
 * @note v1.3.0 (2026-04-05): Up to 3,000,000
 *       1. Extended the upper limit of integers from 2,000,000 to 3,000,000
 *       2. Expanded the memory that holds divisors
 *          - Note: d(2882880)=336 is the maximum within the range 0-3,000,000,
 *                  so the array size is set to 344. (approx. 4.2GB)
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

#define N_MAX (4000000)
//#define N_MAX (1000000)
#define M_MAX (N_MAX)
#define DSP_MAX (128)

uint32_t *divs_pool;

typedef struct {
	uint32_t pool_ofs;
	uint32_t pool_cnt;
	int cnt;
} DIVS, *pDIVS;
pDIVS divs;

int benchmark_mode = 0;

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[])
{
	uint64_t m;
	uint64_t n;
	uint64_t d;
	int ret = 0;
	uint64_t ofs;
	uint64_t pre;
	struct rusage r_start, r_end;
	struct timeval wall_start, wall_end;

	/*--- check argv ---*/
	for (int i = 1; i < argc; i++) {
    	if (strcmp(argv[i], "--benchmark") == 0) {
        	benchmark_mode = 1;
 	   }
	}

	/*--- alloc divs ---*/
	divs = calloc(N_MAX+1, sizeof(DIVS));
	if (divs == NULL) {
		printf("ERR: divs(0) = calloc(%d, %ld)\n", N_MAX+1, sizeof(DIVS));
		return -1;
	}
//	printf("sizeof(DIVS) = %ld\n", sizeof(DIVS));
//	printf("sizeof(DIVS)*(N_MAX+1) = %ld\n", sizeof(DIVS)*(N_MAX+1));

	/*--- make pool_cnt ---*/
	for (m = 1; m <= M_MAX; m++) {
		for (n = m; n <= N_MAX; n += m) {
			divs[n].pool_cnt++;
		}
	}

	/*--- make pool_ofs ---*/
	ofs = 0;
	for (n = 1; n <= N_MAX; n++) {
		divs[n].pool_ofs = ofs;
		ofs += divs[n].pool_cnt;
		ofs++;	// for NULL padding.
	}

	/*--- alloc pool ---*/
	divs_pool = calloc(ofs, sizeof(uint32_t));
	if (divs_pool == NULL) {
		printf("ERR: divs_pool(0) = calloc(%d, %ld)\n", ofs, sizeof(uint32_t));
		return -2;
	}
//	printf("ofs = %ld\n", ofs);
//	printf("ofs*sizeof(uint32_t) = %ld\n", ofs*sizeof(uint32_t));

	/*--- get start time ---*/
	if (benchmark_mode) {
		gettimeofday(&wall_start, NULL);
		getrusage(RUSAGE_SELF, &r_start);
	}

	/*--- Sieve of Eratosthenes ---*/
	for (m = 1; m <= M_MAX; m++) {
		for (n = m; n <= N_MAX; n += m) {
			if (divs[n].cnt < divs[n].pool_cnt) {
				divs_pool[divs[n].pool_ofs+divs[n].cnt] = m;
			}
			divs[n].cnt++;
		}
	}

	/*--- get end time ---*/
	if (benchmark_mode) {
		gettimeofday(&wall_end, NULL);
		getrusage(RUSAGE_SELF, &r_end);
	}

	if (benchmark_mode) {
		/*--- print benchmark ---*/
		double wall = (wall_end.tv_sec - wall_start.tv_sec)
            		    + (wall_end.tv_usec - wall_start.tv_usec) / 1e6;
		double user = (r_end.ru_utime.tv_sec  - r_start.ru_utime.tv_sec)
            		    + (r_end.ru_utime.tv_usec - r_start.ru_utime.tv_usec) / 1e6;
		double sys  = (r_end.ru_stime.tv_sec  - r_start.ru_stime.tv_sec)
            		    + (r_end.ru_stime.tv_usec - r_start.ru_stime.tv_usec) / 1e6;

		printf("real %.3fs user %.3fs  sys %.3fs\n", wall, user, sys);
	}
	else {
		/*--- print divisor stars ---*/
		printf("      n:   d(n):divisors2(n, %u)\n", DSP_MAX);
		printf("%7u:%7d:", 0, N_MAX);
		for (m = 1; m <= DSP_MAX; m++) {
			printf("*");
		}
		printf("...\n");
		for (n = 1; n <= N_MAX; n++) {
			printf("%7lu:%7d:", n, divs[n].cnt);
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
	}

	free(divs_pool);
	free(divs);

	return ret;
}
