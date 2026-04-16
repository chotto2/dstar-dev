#ifndef DIVS_H
#define DIVS_H

extern uint32_t n_max;

#define N_MAX n_max
#define N_MAX_LIMIT (10000000UL)
#define M_MAX (N_MAX)
#define DSP_MAX (128)

#define ERR_OK		(0)
#define ERR_ARGSTYPE	(-1)
#define ERR_ARGC	(-2)
#define ERR_ILLGPARM	(-3)
#define ERR_RANGOVER	(-4)
#define ERR_DIVSALOC	(-5)
#define ERR_POOLALOC	(-6)

typedef struct {
	uint32_t pool_ofs;
	uint32_t pool_cnt;
	uint32_t cnt;
} DIVS;

#endif	// for DIVS_H

