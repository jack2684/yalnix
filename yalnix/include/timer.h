#ifndef _TIMER_H
#define _TIMER_H

#include "hardware.h"
#include "memory.h"
#include "common.h"
#include "dlist.h"

#define DEFAULT_QUANTUMN 2

typedef struct Timer {
    int tick;
    int round_robin_quantumn;
} ytimer_t;

extern ytimer_t timer;

void timer_init();
int round_robin_timeout();

#endif

