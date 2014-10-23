#include "timer.h"

ytimer_t timer;

void timer_init(void) {
    timer.round_robin_quantumn = DEFAULT_QUANTUMN;
    timer.tick = timer.round_robin_quantumn;
}

int round_robin_timeout(void) {
    timer.tick--;
    if(!timer.tick) {
        timer.tick = timer.round_robin_quantumn;
        return 1;
    } else {
        return 0;
    }
}

