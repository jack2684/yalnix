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
    timer_init();
    
    ready_queue = dlist_init(kernel_proc);
    en_ready_queue(user_proc);
}

void DoDoIdle(void) {
    while (1) {
        _debug("Doing DoIdle in kernel proc\n");
        Pause();
    }   
    return;
}

void init_kernel_proc(void) {
    kernel_proc = (pcb_t*) malloc(sizeof(pcb_t));
    bzero(kernel_proc, sizeof(pcb_t));
    kernel_proc->pid = 0;
    kernel_proc->user_context.pc = DoDoIdle;
    kernel_proc->user_context.sp = (void*)(VMEM_0_LIMIT - WORD_LEN / 8);
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

pcb_t* rm_ready_queue(pcb_t *proc) {
    pcb_t *about_to_run = dlist_rm_this(ready_queue, proc->list_node);
    about_to_run->state = RUN;
    running_proc = about_to_run;
}

pcb_t* de_ready_queue() {
    pcb_t *about_to_run = dlist_rm_head(ready_queue);
    if(about_to_run == NULL) {
        log_info("Ready queue is empty, suck it.");
        return NULL;
    }
    return about_to_run;
}

void stall_running_and_en_ready_queue(UserContext *user_context) {
    // Swtich out the running proc
    en_ready_queue(running_proc);
    memcpy(&(running_proc->user_context), user_context, sizeof(UserContext));
    memcpy(&(running_proc->page_table), user_page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    memcpy(&(running_proc->mm), &user_memory, sizeof(vm_t));
    
    // Switch in the kernel idle proc
    memcpy(user_context, &(kernel_proc->user_context), sizeof(UserContext));
}

/* Round robin schedule 
 *  1. Enqueue the running proc
 *  2. Dequeue the next proc in ready queue
 *  4. If the process is delaying, decrement the tick and switch to kernel proc
 */
void round_robin_schedule(UserContext *user_context) {
    if(!ready_queue->size)
        return;
   
    stall_running_and_en_ready_queue(user_context);
    pcb_t *next_proc;
    next_proc = de_ready_queue();
    if(next_proc->remaining_clock_ticks > 0) {
        next_proc->remaining_clock_ticks--;
    } else {
        memcpy(user_context, &(next_proc->user_context), sizeof(UserContext));
        memcpy(user_page_table, &(next_proc->page_table), sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
        memcpy(&user_memory, &(next_proc->mm), sizeof(vm_t));
    }
}

