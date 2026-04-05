/**
 * @file dstar.c
 * @brief Plot of divisors using the Sieve of Eratosthenes.
 * @author N.Arai
 * @date 2026-01-26
 *
 * This program plots the divisors of integers from 0 to n using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
 *
 * @note v1.3.0 (2026-04-05): Up to 3,000,000
 *       1. Extended the upper limit of integers from 2,000,000 to 3,000,000
 *       2. Expanded the memory that holds divisors
 *          - Note: d(2948400)=300 is the maximum within the range 0–3,000,000, so the array size is set to 304. (approx. 3.6GB)
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

#define D_MAX (304)
#define N_MAX (3000000)
#define M_MAX (N_MAX)
#define DSP_MAX (128)

typedef struct {
	uint32_t div[D_MAX];
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

	/*--- check argv ---*/
	for (int i = 1; i < argc; i++) {
    	if (strcmp(argv[i], "--benchmark") == 0) {
        	benchmark_mode = 1;
 	   }
	}

	/*--- divs[N_MAX+1] ---*/
	divs = calloc(N_MAX+1, sizeof(DIVS));
	if (divs == NULL) {
		printf("ERR: NULL = calloc(%d, %d)\n", N_MAX+1, sizeof(DIVS));
		return -1;
	}

	/*--- Sieve of Eratosthenes ---*/
	for (m = 1; m <= M_MAX; m++) {
		for (n = m; n <= N_MAX; n += m) {
			divs[n].div[divs[n].cnt] = m;
			divs[n].cnt++;
		}
	}

	/*--- for printing ---*/
	if (!benchmark_mode) {
		printf("      n:   d(n):divisors2(n, %u)\n", DSP_MAX);
		printf("%7u:%7d:", 0, N_MAX);
		for (m = 1; m <= DSP_MAX; m++) {
			printf("*");
		}
		printf("...\n");
		for (n = 1; n <= N_MAX; n++) {
			printf("%7lu:%7d:", n, divs[n].cnt);
			pre = 0;
			for (ofs = 0; ofs < D_MAX; ofs++) {
				m = divs[n].div[ofs];
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

	free(divs);

	return ret;
}
