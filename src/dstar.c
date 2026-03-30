/**
 * @file dstar.c
 * @brief Plot of divisors using the Sieve of Eratosthenes.
 * @author N.Arai
 * @date 2026-01-26
 *
 * This program plots the divisors of integers from 0 to n using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
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

#define D_MAX (256)
#define N_MIN (1)
#define N_MAX (1000000)
#define M_MAX (N_MAX)
#define DSP_MAX (128)

typedef struct {
	uint32_t div[D_MAX];
	int cnt;
} DIVS, *pDIVS;
DIVS divs[N_MAX+1] = {0};

/**
 * @brief Main entry point
 */
int main()
{
	uint64_t m;
	uint64_t n;
	uint64_t d;
	int ret = 0;
	uint64_t ofs;
	uint32_t count;
	uint64_t pre;

	for (m = 1; m <= M_MAX; m++) {
		// for top position.
		if ((N_MIN % m) == 0) {
			n = N_MIN;
		}
		else {
			n = N_MIN / m;
			n = n * m;
			n += m;
		}

		// for stars.
		for ( ; n <= N_MAX; n += m) {
			if (m < D_MAX) {
				count = divs[n-N_MIN].cnt;
				divs[n-N_MIN].div[count] = m;
			}
			divs[n-N_MIN].cnt++;
		}
	}

	// for printing.
	printf("      n:   d(n):divisors2(n, %u)\n", DSP_MAX);
	printf("%7u:%7d:", 0, N_MAX);
	for (m = 1; m <= DSP_MAX; m++) {
		printf("*");
	}
	printf("...\n");
	for (n = N_MIN; n <= N_MAX; n++) {
		printf("%7lu:%7d:", n, divs[n-N_MIN].cnt);
		pre = 0;
		for (ofs = 0; ofs < D_MAX; ofs++) {
			m = divs[n-N_MIN].div[ofs];
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
	return ret;
}
