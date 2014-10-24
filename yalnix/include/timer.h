#ifndef _TIMER_H
#define _TIMER_H

#include "common.h"
#include "proc.h"
#include "dlist.h"

#define DEFAULT_QUANTUMN 2

typedef struct Timer {
    int tick;
    int round_robin_quantumn;
} ytimer_t;

extern ytimer_t timer;
extern dlist_t *delay_queue;

void timer_init();
int round_robin_timeout();
void ticking_down();
int en_delay_queue(pcb_t *proc);

#endif

