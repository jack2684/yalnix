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
void init_processes() {
    timer_init();
    ready_queue = dlist_init();
    if(!ready_queue) {
        log_err("Cannot init ready queue!");
    }
}

/* A dummy kernel idle proc
 */
void DoDoIdle(void) {
    while (1) {
        _debug("Idling...\n");
        Pause();
    }   
    return;
}

/* Init a dummy idle proc
 */
//void init_idle_proc(UserContext* user_context) {
//    idle_proc = (pcb_t*) malloc(sizeof(pcb_t));
//    if(!idle_proc) {
//        log_err("Cannot malloc idle proc!");
//        return;
//    }
//    bzero(idle_proc, sizeof(pcb_t));
//    idle_proc->user_context.pc = DoDoIdle;
//    idle_proc->user_context.sp = (void*)(VMEM_0_LIMIT - WORD_LEN / 8);
//    idle_proc->kernel_stack_pages = (pte_t*) malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
//    idle_proc->pid = 0;
//    idle_proc->init_done = 0;
//    next_pid = 1;
//
//    save_user_runtime(idle_proc, user_context);
//    init_process_kernel(idle_proc); 
//    return;
//}

/* A general function to initialize user proc
 *
 * @return: A pointer to the newly created pcb;
 *          NULL if creation fails
 */
pcb_t *init_user_proc(void) {
    // Create pcb
    pcb_t *user_proc = (pcb_t*) malloc(sizeof(pcb_t));
    if(!user_proc) {
        log_err("Cannot malloc user proc!");
        return NULL;
    }
    bzero(user_proc, sizeof(pcb_t));
    
    // Create page table
    user_proc->page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    if(!user_proc->page_table) {
        log_err("user_proc->page_table cannot be malloc!");
        return NULL;
    }
    bzero(user_proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    
    // Create kernel stack page table
    user_proc->kernel_stack_pages = (pte_t*) malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    if(!user_proc->kernel_stack_pages) {
        log_err("user_proc->kernel_stack_pages cannot be malloc!");
        return NULL;
    }
    bzero(user_proc->kernel_stack_pages, sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
   
    // Init vitals
    user_proc->init_done = 0;
    user_proc->pid = next_pid;
    next_pid = (next_pid + 1);
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
    log_info("En ready_queue with PID(%d)", proc->pid);
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
    //print_page_table(proc->page_table, 0, 11);
}

/* Copy runtime info
 *
 * @param dest_proc: process copy to
 * @param dest_proc: process copy from
 * @param user_context: user context
 */
int copy_user_runtime(pcb_t *dest_proc, pcb_t *src_proc, UserContext *user_context) {
    save_user_runtime(src_proc, user_context); 
    dest_proc->user_context = src_proc->user_context;
    int rc = alloc_frame_and_copy(dest_proc->page_table, 
                                src_proc->page_table, 
                                0, GET_PAGE_NUMBER(VMEM_1_SIZE), 
                                kernel_memory.swap_addr);
    if(rc) {
        log_err("PID(%d) cannot alloc or copy data from PID(%d) page table", dest_proc->pid, src_proc->pid);
        return 1;
    }
    dest_proc->mm = src_proc->mm;
    return 0;
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
    save_user_runtime(proc, user_context);
    en_ready_queue(proc);
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
    context_switch_to(next_proc, user_context);
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
    // Don't push running_proc into ready quueue if it is a idle proc
    log_info("Round robin with queue size %d, when running PID(%d)", ready_queue->size, running_proc->pid);
    if(!ready_queue->size) {
        return;
    }   
    if(running_proc != idle_proc) {
        save_and_en_ready_queue(running_proc, user_context);
    } else {
        save_user_runtime(running_proc, user_context);   
    }
    
    next_schedule(user_context);
    log_info("Round robin done with queue size %d, now running PID(%d)", ready_queue->size, running_proc->pid);
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
    } else {
        next_proc = de_ready_queue();
        log_info("De ready_queue get PID(%d)", next_proc->pid);
    }
    
    restore_user_runtime(next_proc, user_context);
    context_switch_to(next_proc, user_context);
}
/* Grow or shrink usre stack by doign page management
 *
 * @param proc: the corresponding process
 * @param addr: the new address the trigers
 */
int user_stack_resize(pcb_t *proc, uint32 new_addr) {
    if(!proc) {
        log_err("Process not found for stask resize");
        return 1;
    }
    if(proc == running_proc) {
        proc->mm = user_memory;
    }
    if(proc->mm.brk_high + PAGESIZE >= new_addr) {
        log_err("Stack to low. new addr %p, while user brk high %p. Red zone err!!!", new_addr, proc->mm.brk_high);
        return 1;
    }
    if(VMEM_1_LIMIT < new_addr) {
        log_err("Stack shrink beyond 0.");
        return 1;
    }

    int rc = 0;
    if(new_addr > proc->mm.stack_low) {
        rc = unmap_page_to_frame(proc->page_table,
                                USER_PAGE_NUMBER(proc->mm.stack_low),
                                USER_PAGE_NUMBER(new_addr));
        if(rc) {
            log_err("Cannot shrink user stack");
            return 1;
        }
    } else if (new_addr < proc->mm.stack_low) {
        rc = map_page_to_frame(proc->page_table,
                                USER_PAGE_NUMBER(new_addr),
                                USER_PAGE_NUMBER(proc->mm.stack_low),
                                PROT_READ | PROT_WRITE);
        if(rc) {
            log_err("Cannot enlarge user stack");
            return 1;
        } 
    }
    log_info("Resize PID(%d) stack low from %p to %p", proc->pid, proc->mm.stack_low, new_addr);
    proc->mm.stack_low = new_addr;
    return 0;
}

/* Determine whether a process is dead
 */
int is_proc_avtive(pcb_t *proc) {
    return proc && proc->state != ZOMBIE && proc->state != EXIT;
}

/* Wrapper of kernel context switch
 *
 * @param next_proc: the process to be switched in
 * @param user_context: current user context
 */
void context_switch_to(pcb_t *next_proc, UserContext *user_context) {
    int rc = 0;
    rc = KernelContextSwitch(&kernel_context_switch, running_proc, next_proc);
    if(rc) {
        log_err("Failed to execute magic function!");
        Halt();
    }
}

void init_process_kernel(pcb_t *proc) {
    int rc = 0;
    
    rc = KernelContextSwitch(&init_newbie_kernel, proc, proc);
    if(rc) {
        log_err("Failed to execute magic function!");
        Halt();
    }
    log_info("Init PID(%d) kernel stack done", proc->pid);
}

KernelContext *init_newbie_kernel(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb){
    pcb_t *next_proc = (pcb_t *) _next_pcb;
    log_info("First time to init PID(%d) kernel stack!", next_proc->pid);
   
    if(next_proc->kernel_stack_pages == NULL) {
        log_err("Init kernel stack fail, pcb->kernel_stack_pages not malloc yet");
        Halt();
    }

    memcpy(next_proc->kernel_stack_pages, 
            &kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], 
            sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    next_proc->init_done = 1;

    log_info("First time to init PID(%d) kernel stack done", next_proc->pid);
    return kernel_context;
}

/* Caller kernel context switch magic function,
 * 1. Back up the current kernel context
 * 2. Restore kernel stack and do the corresponding TLB flush
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

    // Backup current kernel context, and set next running process
    prev_proc->kernel_context = *kernel_context;
    running_proc = next_proc;
    
    if(next_proc->init_done == 0) {
        // If just initialized (like just forked), init the context and kernel stack
        log_info("Init next proc PID(%d)!", next_proc->pid);
        next_proc->init_done = 1;
        next_proc->kernel_context = *kernel_context;
        int rc = alloc_frame_and_copy(next_proc->kernel_stack_pages, 
                                    kernel_page_table, 
                                    GET_PAGE_NUMBER(KERNEL_STACK_BASE), 
                                    GET_PAGE_NUMBER(KERNEL_STACK_LIMIT), 
                                    kernel_memory.swap_addr);
        if(rc) {
            log_err("PID(%d) kernel stack cannot init", next_proc->pid);
            return NULL;
        }
    }

    // Load kernel stack from next processs and flush corresponding TLB
    int addr;
    memcpy(&kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], 
            next_proc->kernel_stack_pages, 
            sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE );
    //WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
    for(addr = KERNEL_STACK_BASE; addr < KERNEL_STACK_LIMIT; addr += PAGESIZE) {
        //log_info("Kernel Context Switch flushing addr: %p", addr);
        WriteRegister(REG_TLB_FLUSH, addr);
    }

    log_info("Magic kernel switch done from PID(%d) to PID(%d)", prev_proc->pid, next_proc->pid);
    *kernel_context  = next_proc->kernel_context;
    return kernel_context;
}

