/**
 * @file dstar.c
 * @brief Plot the divisors up to 1,000,000.
 * @author N.Arai
 * @date 2026-01-26
 *
 * This program plots the divisors of integers from 0 to 1,000,000 using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
 *
 * @note Do not display if the number of divisors exceeds 128.
 * @see Related paper on arXiv (to be published)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define D_MAX (128)
#define N_MIN (0)
#define N_MAX (1000000ULL)
#define M_MAX (N_MAX)

typedef struct {
	unsigned long long div[D_MAX/64];
	int cnt;
} DIVS, *pDIVS;
DIVS divs[N_MAX+1];

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
			divs[n-N_MIN].cnt++;
			if (m < D_MAX) {
				ofs = m - 1;
				divs[n-N_MIN].div[ofs / 64] |= (1ULL << (ofs % 64));
			}
		}
	}

	// for printing.
	printf("      n:   d(n):divisors2(n, %llu)\n", M_MAX);
	for (n = N_MIN; n <= N_MAX; n++) {
		printf("%7llu:%7d:", n, divs[n-N_MIN].cnt);
		for (d = 1; d <= D_MAX; d++) {
			ofs = d - 1;
			printf("%s", (divs[n-N_MIN].div[ofs / 64] & (1ULL << ((ofs %64))))? "*":" ");
		}
		printf("%s", (divs[n-N_MIN].cnt >= D_MAX)? "...": "");
		printf("\n");
	}
	return ret;
}
