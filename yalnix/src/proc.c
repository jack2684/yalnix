#include "proc.h"

pcb_t   *kernel_proc;          // A kernel proc
pcb_t   *user_proc;          // A user proc
pcb_t   *running_proc;      // Current running proc
dlist_t  *ready_queue;   
dlist_t  *wait_queue;

/* Init the pcb of two original processes, 
 * which run in user and kernel land respectively
 */
void init_pcb(void) {
    kernel_proc = (pcb_t*) malloc(sizeof(pcb_t));
    user_proc = (pcb_t*) malloc(sizeof(pcb_t));
    bzero(kernel_proc, sizeof(pcb_t));
    bzero(user_proc, sizeof(pcb_t));

    kernel_proc->pid = 0;
    user_proc->pid = 1;

    running_proc = user_proc;
    ready_queue = dlist_init(kernel_proc);
    en_ready_queue(ready_queue, kernel_proc);
}

int en_ready_queue(void *proc) {
    node_t *n = dlist_add_tail(ready_queue, proc);
    if(!n) {
        _debug("Cannot enqueue the pcb\n");
        return 1;
    }
    proc->dlist_node = n; 
}

void* de_ready_queue(pcb_t *proc) {
    return dlist_rm_this(ready_queue, proc->list_node);
}

void init_kernel_proc(void) {
    
}

