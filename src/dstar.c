/**
 * @file dstar.c
 * @brief Plot of divisors using the Sieve of Eratosthenes.
 * @author N.Arai
 * @date 2026-04-15
 *
 * This program plots the divisors of integers from 0 to n using asterisks. 
 * This allows you to understand the overall distribution pattern of divisors among integers.
 *
 * @note v2.0.0 (2026-04-15): Up to 10,000,000
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
#include <errno.h>
#include "benchmark.h"
#include "divs.h"

uint32_t n_max;
static int memory_mode = 0;
static int benchmark_mode = 0;
static uint64_t total_memory;

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
 *
 * @return 0 on success, negative error code on failure:
 *        -1: A non-numeric value was specified as the first argument
 *        -2: No arguments were specified
 *        -3: An illegal argument was specified
 *        -4: Exceeded the upper limit of n_max
 *        -5: Failed to allocate divisor structures
 *        -6: Failed to allocate divisor pool
 *        -7: Show version only
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
				if (memory_mode) {
					printf("total memory = %lu\n", total_memory);
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
				}

				free(divs_pool);
				ret = ERR_OK;
			}

			free(divs);
		}
	}

	return ret;
}

/**
 * @brief Display the usage of the command
 */
static void print_usage()
{
	printf("USAGE: dstar { {-v | --version} | <n_max> [{-m | --memory}] [{-b | --benchmark}] }\n");
	printf("\n");
	printf("  -v, --version    Show version number\n");
	printf("  n_max            Upper limit for divisor computation (positive integer)\n");
	printf("  -m, --memory     Show memory required for n_max and exit (no computation)\n");
	printf("                   (takes precedence over -b if both are specified)\n");
	printf("  -b, --benchmark  Show elapsed/user/sys time after computation\n");
}

/**
 * @brief Check the command arguments
 *
 * @return 0 on success, negative error code on failure:
 *        -1: A non-numeric value was specified as the first argument
 *        -2: No arguments were specified
 *        -3: A illegal argument was specified
 *        -4: Exceeded the upper limit of n_max
 *
 */
static int check_arg(int argc, char *argv[])
{
	int ret = ERR_OK;

	if (argc < 2) {
		print_usage();
		ret = ERR_ARGC;
	}
	else {
		for (int i = 1; i < argc; i++) {
			if (i == 1) {
				if ((strcmp(argv[i], "--version") == 0)
				||  (strcmp(argv[i], "-v") == 0)) {
					printf("version: %s\n", VERSION);
					ret = ERR_VERSION;
					break;
				}
				else {
					if (is_digits(argv[i])) {
						errno = 0;
						unsigned long val = strtoul(argv[1], NULL, 10);
						if (errno == ERANGE || val == 0 || val > N_MAX_LIMIT) {
    							printf("ERR: n_max out of range\n");
							print_usage();
    							ret = ERR_RANGOVER;
							break;
						}
						else {
							n_max = (uint32_t)val;
						}
					}
					else {
						print_usage();
						ret = ERR_ARGSTYPE;
						break;
					}
				}
			}
			else {
				if ((strcmp(argv[i], "--memory") == 0)
				||  (strcmp(argv[i], "-m") == 0)) {
					memory_mode = 1;
				}
				else {
					if ((strcmp(argv[i], "--benchmark") == 0)
					||  (strcmp(argv[i], "-b") == 0)) {
						benchmark_mode = 1;
					}
					else {
						print_usage();
						ret = ERR_ILLGPARM;
						break;
					}
				}
			}
		}
	}

	return ret;
}

/**
 * @brief Allocate several structures that indirectly point to the list of divisors
 *
 * @return Address of the allocated structures, NULL on error
 *
 */
static DIVS *calloc_divs()
{
	DIVS *p;

	p = calloc(N_MAX+1, sizeof(DIVS));
	if (p == NULL) {
		printf("ERR: divs(0) = calloc(%u, %ld)\n", N_MAX+1, sizeof(DIVS));
	}
	total_memory = sizeof(DIVS)*(N_MAX+1);

	return p;
}

/**
 * @brief Set the position and number in the pool
 *
 * @param[in]  divs Refers to a part of the pool for each integer
 *
 * @return  Total number of words in the pool to allocate
 *
 */
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

/**
 * @brief Allocate memory area to hold a list of divisors
 *
 * @param[in]  num  Number of words in the list of all divisors
 *                  - Note: including NULL padding
 *
 * @return Address of the allocated pool, NULL on error
 *
 */
static uint32_t *calloc_pool(uint32_t num)
{
	uint32_t *p;

	p = calloc(num, sizeof(uint32_t));
	if (p == NULL) {
		printf("ERR: divs_pool(0) = calloc(%u, %ld)\n", num, sizeof(uint32_t));
	}
	total_memory += sizeof(uint32_t)*num;

	return p;
}

/**
 * @brief Display the divisors in the table
 *
 * @param[in]  divs       Refers to the list of divisors within the pool
 * @param[in]  divs_pool  A block of NULL-padded divisor lists
 *
 */
static void print_divisor_stars(DIVS *divs, uint32_t *divs_pool)
{
	uint32_t m;
	uint32_t n;
	uint32_t pre;
	uint32_t ofs;
	int width = snprintf(NULL, 0, "%u", N_MAX);

	width = (width < 7)? 7: width;		// for compatibility
	printf("%*s:%*s:divisors2(n, %u)\n", width, "n", width, "d(n)",  DSP_MAX);
	printf("%*u:%*u:", width, 0, width, N_MAX);
	for (m = 1; m <= DSP_MAX; m++) {
		printf("*");
	}
	printf("...\n");
	for (n = 1; n <= N_MAX; n++) {
		printf("%*u:%*u:", width, n, width, divs[n].cnt);
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

