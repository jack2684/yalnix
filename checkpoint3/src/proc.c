#include "proc.h"

pcb_t   *kernel_proc;          // A kernel proc
pcb_t   *idle_proc;          // A idle user proc
pcb_t   *running_proc;      // Current running proc
dlist_t  *ready_queue;   
dlist_t  *wait_queue;

int next_pid;

/* Init the pcb of two original processes, 
 * which run in user and kernel land respectively
 */

/* Init basic process management, timer and a dummy kernel proc
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

/* A dummy kernel idle proc
 */
void DoDoIdle(void) {
    while (1) {
        _debug("Doing DoIdle in kernel proc\n");
        Pause();
    }   
    return;
}

/* Init a dummy kernel proc
 */
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
    kernel_proc->kernel_stack_pages = NULL;
    return;
}

/* A general function to initialize user proc
 *
 * @return: A pointer to the newly created pcb;
 *          NULL if creation fails
 */
pcb_t *init_user_proc(void) {
    pcb_t *user_proc = (pcb_t*) malloc(sizeof(pcb_t));
    if(!kernel_proc) {
        log_err("Cannot malloc user proc!");
        return NULL;
    }
    bzero(user_proc, sizeof(pcb_t));
    user_proc->pid = next_pid;
    next_pid = (next_pid + 1);
    user_proc->page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    if(!user_proc->page_table) {
        log_err("user_proc->page_table cannot be malloc!");
        return NULL;
    }
    user_proc->kernel_stack_pages = (pte_t*) malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    if(!user_proc->kernel_stack_pages) {
        log_err("user_proc->kernel_stack_pages cannot be malloc!");
        return NULL;
    }
    bzero(user_proc->kernel_stack_pages, sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    return user_proc;
}

/* Enqueue a pcb into ready queue,
 * set its state to ready,
 * and keep a pointer to its inner queue node
 *
 * @param proc: the pcb to be enqueued
 * @return: A pointer to the newly created pcb;
 *          NULL if creation fails
 */
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

/* Safe user land runtime info
 *
 * @param proc: the pcb to be saved
 * @param user_context: current user context
 */
void save_user_runtime(pcb_t *proc, UserContext *user_context) {
    proc->user_context = *user_context;
    //memcpy(proc->page_table, user_page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    proc->mm = user_memory;
}

/* Safe user land runtime info
 *
 * @param proc: the pcb to be restored
 * @param user_context: current user context
 */
void restore_user_runtime(pcb_t *proc, UserContext *user_context) {
    *user_context = proc->user_context;
    set_user_page_table(proc->page_table);
    user_memory = proc->mm;
}

/* Safe user land runtime info and push into queue
 *
 * @param proc: the pcb to be restored
 * @param user_context: current user context
 */
void save_and_en_ready_queue(pcb_t *proc, UserContext *user_context) {
    en_ready_queue(proc);
    save_user_runtime(proc, user_context);
}

/* Remove a specified proc from the queue 
 */
pcb_t* rm_ready_queue(pcb_t *proc) {
    pcb_t *about_to_run = dlist_rm_this(ready_queue, proc->list_node);
}

/* Dequeue
 *
 * @return: return the head proc of ready queue, 
 *          return NULL if dequeue fails 
 */
pcb_t* de_ready_queue() {
    pcb_t *about_to_run = dlist_rm_head(ready_queue);
    if(about_to_run == NULL) {
        log_info("Ready queue is empty, suck it.");
        return NULL;
    }
    return about_to_run;
}

/* Dueue and run the first process
 * noted that seems this is only for the first process, 
 * I am not using context switch
 */
pcb_t* de_ready_queue_and_run(UserContext *user_context) {
    pcb_t *next_proc = de_ready_queue();
    restore_user_runtime(next_proc, user_context);
    memcpy(next_proc->kernel_stack_pages, &kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    running_proc = next_proc;
    switch_to_process(next_proc, user_context);
    return next_proc;
}

/* Round robin schedule 
 *  1. Enqueue the running proc, if idle is running, don't enqueue
 *  2. Dequeue the next proc in ready queue
 *  3. If the process is delaying, decrement the tick and switch to kernel proc
 *
 *  @param user_context: current user context
 */
void round_robin_schedule(UserContext *user_context) {
    //log_info("Round robin with ready queue size: %d", ready_queue->size);
    
    // Don't round robin if there is no one in ready queue
    if(!ready_queue->size) {
        return;
    }
 
    // Don't push running_proc into ready quueue if it is a idle proc
    if(!ready_queue->size) {
        return;
    }   
    if(running_proc != idle_proc) {
        save_and_en_ready_queue(running_proc, user_context);
    } //else {
    //    save_user_runtime(running_proc, user_context);   
    //}
    
    next_schedule(user_context);
}

/* Pick any process in the ready queue to run
 * if ready queue is empty, run the idle process
 *
 * @param user_context: current user context
 */
void next_schedule(UserContext *user_context) {
    pcb_t *next_proc;    
    pcb_t *proc = running_proc;
    if(!ready_queue->size) {
        next_proc = idle_proc;
        save_user_runtime(running_proc, user_context);
    } else {
        next_proc = de_ready_queue();
    }
    
    restore_user_runtime(next_proc, user_context);
    switch_to_process(next_proc, user_context);
    running_proc = next_proc;
}

/* Wrapper of kernel context switch
 *
 * @param next_proc: the process to be switched in
 * @param user_context: current user context
 */
void switch_to_process(pcb_t *next_proc, UserContext *user_context) {
    //log_info("Before set context");
    *user_context = next_proc->user_context;

    //log_info("Before set running");
    pcb_t *prev_proc = running_proc;
    running_proc = next_proc;

    // Using the magic function
    //log_info("Before magic");
    int rc = KernelContextSwitch(&kernel_context_switch, prev_proc, next_proc);
    if (_SUCCESS == rc) {
        log_info("get return proc:");
        log_info("Succesfully switched kernel context to PID %d!", running_proc->pid);
    } else {
        log_err("Failed to save kernel context!");
        Halt();
    }
    //log_info("Done magic");
    
    // Load the new user context
    *user_context = running_proc->user_context;
    log_info("Switch Context from PID(%d) to PID(%d) DONE", prev_proc->pid, next_proc->pid);
    //log_info("Done context swtiching");
}

/* Caller kernel context switch magic function,
 * 1. Back up the current kernel context
 * 2. Init kernel stack if not  
 * 3. Restore kernel stack and do the corresponding TLB flush
 *
 * @param kernel_context: the mysterious kernel context
 * @param _prev_proc: the process to be switched out
 * @param _next_proc: the process to be switched in
 * @return: kernel context
 */
KernelContext *kernel_context_switch(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb)
{
    pcb_t *prev_proc = (pcb_t *) _prev_pcb;
    pcb_t *next_proc = (pcb_t *) _next_pcb;

    // Backup current kernel context
    if(prev_proc != NULL) {
        memcpy(&prev_proc->kernel_context, kernel_context, sizeof(KernelContext));
        memcpy(prev_proc->kernel_stack_pages, &kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], sizeof(next_proc->kernel_stack_pages) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    } 
    // Init kernel stack if needed
    if(next_proc->kernel_stack_pages[0].pfn == 0) { 
        log_info("next_proc->kernel_stack_pages addr initializing %p with PID %d", next_proc->kernel_stack_pages, next_proc->pid);
        memcpy(&next_proc->kernel_context, kernel_context, sizeof(KernelContext));
        map_page_to_frame(next_proc->kernel_stack_pages, 0, KERNEL_STACK_MAXSIZE / PAGESIZE, PROT_READ | PROT_WRITE);
    }

    if (next_proc == idle_proc || prev_proc == idle_proc) {
        return kernel_context; 
    }
   
    log_info("The sizeof(next_proc->kernel_stack_pages) is %d, the page number is %d", sizeof(next_proc->kernel_stack_pages), KERNEL_STACK_MAXSIZE / PAGESIZE );
    log_info("next_proc->kernel_stack_pages %p, &kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)] %p", next_proc->kernel_stack_pages, &kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)]);
    memcpy(&kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], 
            next_proc->kernel_stack_pages, 
            sizeof(next_proc->kernel_stack_pages) * KERNEL_STACK_MAXSIZE / PAGESIZE );
    log_info("Write kernel stack done");
    int addr;
    for(addr = KERNEL_STACK_BASE; addr < KERNEL_STACK_LIMIT; addr += PAGESIZE) {
        WriteRegister(REG_TLB_FLUSH, addr);
    }
    log_info("Flush done");

    *kernel_context  = next_proc->kernel_context;
    return kernel_context;
}

