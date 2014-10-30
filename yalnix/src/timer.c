#include "timer.h"

ytimer_t timer;
dlist_t* delay_queue;

/* init timer
 */
void timer_init(void) {
    timer.round_robin_quantumn = DEFAULT_QUANTUMN;
    timer.tick = timer.round_robin_quantumn;
    delay_queue = dlist_init();
}

/* Ticking down the round robin quantumn
 */
int round_robin_timeout(void) {
    timer.tick--;
    if(!timer.tick) {
        timer.tick = timer.round_robin_quantumn;
        return 1;
    } else {
        return 0;
    }
}

/* Put a pcb into the delay queue
 * @param proc: the process to be put
 */
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

/* Put a pcb into the delay queue
 * @param proc: the process to be put
 */
int de_delay_queue(pcb_t *proc) {
    dlist_rm_this(delay_queue, proc->list_node);
    return _SUCCESS;
}

/* Ticking down the delay time of each process
 * if a process reach 0, it should be remove from 
 * delay queue and push into ready queue
 */
void ticking_down(){
    if(!delay_queue->size) {
        return;
    }   
    dnode_t* node = delay_queue->head;
    while(node) {
        pcb_t* proc = (pcb_t*)node->data;
        log_info("Ticking down PID(%d)", proc->pid);
        node = node->next;
        if(!proc->ticks) {
            log_info("PID(%d) back to life!");
            dlist_rm_this(delay_queue, proc->list_node);
            en_ready_queue(proc); 
        } else {
            proc->ticks = proc->ticks == 0 ? 0 : proc->ticks -1; 
        }
    }   
}

