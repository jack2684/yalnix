#include "timer.h"

ytimer_t timer;
dlist_t* delay_queue;

void timer_init(void) {
    timer.round_robin_quantumn = DEFAULT_QUANTUMN;
    timer.tick = timer.round_robin_quantumn;
    delay_queue = dlist_init();
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

int en_delay_queue(pcb_t *proc) {
    dnode_t* n = dlist_add_tail(delay_queue, proc);
    if(!n) {
       log_err("Cannot enqueue delay queue");
       return _FAILURE;
    }
    proc->list_node = n;
    proc->state = PAUSE;
    return _SUCCESS;
}

void ticking_down(){
    if(!delay_queue->size) {
        return;
    }   
    dnode_t* node = delay_queue->head;
    while(node) {
        pcb_t* proc = (pcb_t*)node->data;
        node = node->next;
        if(!proc->ticks) {
            dlist_rm_this(delay_queue, proc->list_node);
            en_ready_queue(proc); 
        } else {
            proc->ticks = proc->ticks == 0 ? 0 : proc->ticks -1; 
        }
    }   
}

