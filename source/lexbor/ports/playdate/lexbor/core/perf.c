/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/perf.h"

#ifdef LEXBOR_WITH_PERF

#include <pd_api.h>
extern PlaydateAPI *pd;

static unsigned long long
lexbor_perf_clock(void);

static unsigned long long
lexbor_perf_frequency(void);


typedef struct lexbor_perf {
    unsigned long long start;
    unsigned long long end;
    unsigned long long freq;
}
lexbor_perf_t;


void *
lexbor_perf_create(void)
{
    lexbor_perf_t *perf = lexbor_calloc(1, sizeof(lexbor_perf_t));
    if (perf == NULL) {
        return NULL;
    }

    perf->freq = lexbor_perf_frequency();

    return perf;
}

void
lexbor_perf_clean(void *perf)
{
    memset(perf, 0, sizeof(lexbor_perf_t));
}

void
lexbor_perf_destroy(void *perf)
{
    if (perf != NULL) {
        lexbor_free(perf);
    }
}

lxb_status_t
lexbor_perf_begin(void *perf)
{
    pd->system->resetElapsedTime();
    ((lexbor_perf_t *) (perf))->start = lexbor_perf_clock();

    return LXB_STATUS_OK;
}

lxb_status_t
lexbor_perf_end(void *perf)
{
    ((lexbor_perf_t *) (perf))->end = lexbor_perf_clock();

    return LXB_STATUS_OK;
}

double
lexbor_perf_in_sec(void *perf)
{
    lexbor_perf_t *obj_perf = (lexbor_perf_t *) perf;

    if (obj_perf->freq != 0) {
        return ((double) (obj_perf->end - obj_perf->start)
                / (double)obj_perf->freq);
    }

    return 0.0f;
}

static unsigned long long
lexbor_perf_clock(void)
{
    unsigned long long x;
    return (unsigned long long)(pd->system->getElapsedTime());
}

static unsigned long long
lexbor_perf_frequency(void)
{
    unsigned long long freq = 168000000;
    return freq;
}

#endif /* LEXBOR_WITH_PERF */
