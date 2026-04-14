#ifndef DIVS_H
#define DIVS_H

#define N_MAX n_max
#define M_MAX (N_MAX)
#define DSP_MAX (128)

#define ERR_OK		(0)
#define ERR_ARGSTYPE	(-1)
#define ERR_ARGC	(-2)
#define ERR_DIVSALOC	(-3)
#define ERR_POOLALOC	(-4)
#define ERR_ILLGPARM	(-5)

typedef struct {
	uint32_t pool_ofs;
	uint32_t pool_cnt;
	uint32_t cnt;
} DIVS;

static uint32_t n_max;
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

#endif	// for DIVS_H

