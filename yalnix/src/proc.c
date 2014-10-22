#include "proc.h"

pcb_t   *kernel_proc;          // A kernel proc
pcb_t   *user_proc;          // A user proc
pcb_t   *running_proc;      // Current running proc
dlist_t  *ready_queue;   
dlist_t  *wait_queue;

/* Init the pcb of two original processes, 
 * which run in user and kernel land respectively
 */
void init_processes(void) {
    init_kernel_proc();
    init_user_proc();

    ready_queue = dlist_init(kernel_proc);
    en_ready_queue(kernel_proc);
    en_ready_queue(user_proc);
de_ready_queue(user_proc);
}

void DoDoIdle(void) {
    while (1) {
        _debug("...... in %s() ....\n", __func__);
        Pause();
    }   
    return;
}

void init_kernel_proc(void) {
    kernel_proc = (pcb_t*) malloc(sizeof(pcb_t));
    bzero(kernel_proc, sizeof(pcb_t));
    kernel_proc->pid = 0;
    kernel_proc->user_context.pc = DoDoIdle;
    kernel_proc->user_context.sp = (void*)kernel_memory.stack_low;
    return;
}

void init_user_proc(void) {
    user_proc = (pcb_t*) malloc(sizeof(pcb_t));
    bzero(user_proc, sizeof(pcb_t));
    user_proc->pid = 1;
    return;
}

int en_ready_queue(pcb_t *proc) {
    dnode_t *n = dlist_add_tail(ready_queue, proc);
    if(!n) {
        _debug("Cannot enqueue the pcb\n");
        return 1;
    }
    proc->list_node = n; 
    proc->state = READY;
    return 0;
}

pcb_t* de_ready_queue(pcb_t *proc) {
    pcb_t *about_to_run = dlist_rm_this(ready_queue, proc->list_node);
    about_to_run->state = RUN;
    running_proc = user_proc;
    return about_to_run;
}

