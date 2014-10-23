#include "proc.h"

pcb_t   *kernel_proc;          // A kernel proc
pcb_t   *user_proc;          // A user proc
pcb_t   *running_proc;      // Current running proc
dlist_t  *ready_queue;   
dlist_t  *wait_queue;

int next_pid;

/* Init the pcb of two original processes, 
 * which run in user and kernel land respectively
 */
void init_processes(void) {
    init_kernel_proc();
    timer_init();
    next_pid = 1;

    ready_queue = dlist_init();
    if(!ready_queue) {
        log_err("Cannot init ready queue!");
    }
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
    if(!kernel_proc) {
        log_err("Cannot malloc kernel proc!");
        return;
    }
    bzero(kernel_proc, sizeof(pcb_t));
    kernel_proc->pid = 0;
    kernel_proc->user_context.pc = DoDoIdle;
    kernel_proc->user_context.sp = (void*)(VMEM_0_LIMIT - WORD_LEN / 8);
    return;
}

pcb_t *init_user_proc(void) {
    user_proc = (pcb_t*) malloc(sizeof(pcb_t));
    if(!kernel_proc) {
        log_err("Cannot malloc user proc!");
        return NULL;
    }
    bzero(user_proc, sizeof(pcb_t));
    user_proc->pid = next_pid;
    next_pid = (next_pid + 1);
    user_proc->page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    return user_proc;
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

void safe_and_en_ready_queue(pcb_t *proc, UserContext *user_context) {
    // Safe the states
    // log_info("START stall_running_and_en_ready_queue with ready queue size: %d", ready_queue->size);
    en_ready_queue(proc);
    // log_info("DONE stall_running_and_en_ready_queue with ready queue size: %d", ready_queue->size);
    memcpy(&(proc->user_context), user_context, sizeof(UserContext));
    memcpy(proc->page_table, user_page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    memcpy(&(proc->mm), &user_memory, sizeof(vm_t));
    // log_info("DONE backup with ready queue size: %d", ready_queue->size);
}

pcb_t* rm_ready_queue(pcb_t *proc) {
    pcb_t *about_to_run = dlist_rm_this(ready_queue, proc->list_node);
    about_to_run->state = RUN;
    running_proc = about_to_run;
}

pcb_t* de_ready_queue() {
    log_info("About to de ready queue, size: %d", ready_queue->size);
    pcb_t *about_to_run = dlist_rm_head(ready_queue);
    if(about_to_run == NULL) {
        log_info("Ready queue is empty, suck it.");
        return NULL;
    }
    log_info("De ready queue successfully");
    return about_to_run;
}

void stall_running_and_en_ready_queue(UserContext *user_context) {
    // Swtich out the running proc
    safe_and_en_ready_queue(running_proc, user_context);

    // Switch in the kernel idle proc
    memcpy(user_context, &(kernel_proc->user_context), sizeof(UserContext));
    running_proc = kernel_proc;
}

pcb_t* de_ready_queue_and_run(UserContext *user_context) {
    log_info("De ready queue");
    pcb_t *next_proc = de_ready_queue();
    log_info("De ready queue DONE");
    memcpy(user_context, &(next_proc->user_context), sizeof(UserContext));
    log_info("Copy uctxt done");
    memcpy(user_page_table, next_proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    log_info("Copy page table done");
    memcpy(&user_memory, &(next_proc->mm), sizeof(vm_t));
    log_info("Copy mem done");
    running_proc = next_proc;
    return next_proc;
}

/* Round robin schedule 
 *  1. Enqueue the running proc
 *  2. Dequeue the next proc in ready queue
 *  4. If the process is delaying, decrement the tick and switch to kernel proc
 */
void round_robin_schedule(UserContext *user_context) {
    //log_info("Round robin with ready queue size: %d", ready_queue->size);
    if(!ready_queue->size) {
        return;
    }
  
    if(running_proc != kernel_proc) {
        stall_running_and_en_ready_queue(user_context);
    }
    pcb_t *next_proc;
    next_proc = de_ready_queue();
    if(next_proc->remaining_clock_ticks > 0) {
        log_info("PID %d keeps delaying with remaining %d", next_proc->pid, next_proc->remaining_clock_ticks);
        next_proc->remaining_clock_ticks--;
        en_ready_queue(next_proc);
    } else {
        log_info("PID %d get back to live!", next_proc->pid);
        memcpy(user_context, &(next_proc->user_context), sizeof(UserContext));
        memcpy(user_page_table, next_proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
        memcpy(&user_memory, &(next_proc->mm), sizeof(vm_t));
        running_proc = next_proc;
    }
}

