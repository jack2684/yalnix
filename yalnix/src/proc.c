/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "proc.h"

pcb_t   *init_proc;          
pcb_t   *idle_proc;
pcb_t   *running_proc;      // Current running proc
dlist_t  *ready_queue;   
dlist_t  *wait_queue;
dlist_t  *write_queue;
dlist_t  *read_queue;

int next_pid;

/* Init the pcb of two original processes, 
 * which run in user and kernel land respectively
 */

/* Init basic process management, timer and a dummy kernel proc
 */
void init_processes() {
    next_pid = 0;
    log_info("Inside %s", __func__);
    timer_init();
    log_info("Init timer done");
    ready_queue = dlist_init();
    wait_queue = dlist_init();
    if(!ready_queue || !ready_queue) {
        log_err("Cannot init ready queue!");
    }
    log_info("Init queue done");
    init_idle_proc();
    log_info("Init idle done");
}

/* A dummy kernel idle proc
 */
void DoDoIdle(void) {
    while (1) {
        user_log("Idling...");
        Pause();
    }   
    return;
}

/* Incrementally get the next PID number
 */
int get_next_pid(){
   if(next_pid >= MAX_PROC) {
        log_err("Exceed %d processes", MAX_PROC);
        next_pid = 11;
   }
   return next_pid++;
}

/* Tell children that I am dead...
 */
void tell_children(pcb_t *proc) {
    dnode_t *node = proc->children->head;
    while(node) {
        pcb_t *child = node->data;
        child->parent = NULL;
        if(child->state == ZOMBIE) {
             free_proc(proc);
        }
    }
}

/* Remove a process from wait queue
 */
pcb_t* rm_wait_queue(pcb_t* proc) {
    return dlist_rm_this(wait_queue, proc->list_node);
}

/* Tell my parent I am dead, in case my parent is waiting
 */
void tell_parent(pcb_t *proc) {
    pcb_t *parent = (pcb_t*)proc->parent;
    if(!parent) {
        log_info("Orphan child do not have parent");
        return;
    }

    if(parent->state == WAIT) {
        log_info("About to free my parent from waiting");
        rm_wait_queue(parent);
        en_zombie_queue(parent, proc);
        en_ready_queue(parent);
    }
}

/* Enqueue the exit_queue of parent, with the exit_code
 */
int en_zombie_queue(pcb_t* parent, pcb_t* child) {
    dnode_t *n = dlist_add_tail(parent->zombie, child);
    if(!n) {
        log_err("Cannot add exit code to parent PID(%d)", parent->pid);
        return 1;
    }
    child->list_node = n;
    child->state = ZOMBIE;
    return 0;
}

/* Completely free the PCB block
 */
void free_proc(pcb_t *proc) {
    int rc, pid = proc->pid;
    rc = unmap_page_to_frame(proc->page_table, 0, GET_PAGE_NUMBER(VMEM_1_SIZE));
    if(rc) {
        log_err("Unable to free frames of PID(%d) page table", proc->pid);
    }
    free(proc->page_table);
    
    unmap_page_to_frame(proc->kernel_stack_pages, 0, GET_PAGE_NUMBER(KERNEL_STACK_MAXSIZE));
    if(rc) {
        log_err("Unable to free frames of PID(%d) kernel stack page table", proc->pid);
    }
    free(proc->kernel_stack_pages);

    free(proc); 
    log_info("PID(%d) is freed", pid);
}

/* Init a dummy idle proc
 */
void init_idle_proc() {
    idle_proc = (pcb_t*) malloc(sizeof(pcb_t));
    if(!idle_proc) {
        log_err("Cannot malloc idle proc!");
        return;
    }
    bzero(idle_proc, sizeof(pcb_t));
    idle_proc->user_context.pc = DoDoIdle;
    idle_proc->user_context.sp = (void *)kernel_memory.stack_low;
    //idle_proc->user_context.ebp = (void *)kernel_memory.stack_low;
    //idle_proc->user_context.code = YALNIX_NOP;
    //idle_proc->user_context.vector = TRAP_KERNEL;
    idle_proc->page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    idle_proc->kernel_stack_pages = (pte_t*) malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    map_page_to_frame(idle_proc->page_table, 0, GET_PAGE_NUMBER(VMEM_1_SIZE), PROT_READ);
    idle_proc->pid = get_next_pid();
    idle_proc->state = READY;
    idle_proc->init_done = 0;

    //init_process_kernel(idle_proc); 
    return;
}

/* Put a process into wait queue, invoked by Wait() syscall
 */
int en_wait_queue(pcb_t *proc) {
    dnode_t *n = dlist_add_tail(wait_queue, proc);
    if(!n) {
        log_err("Cannot add to wait queue");
        return 1;
    }
    proc->list_node = n;
    proc->state = WAIT;
    proc->wait_zombie = 1;
    log_info("PID(%d) en wait_queue", proc->pid);
    return 0;
}

/* Get one zombie child from zombie queue
 */
pcb_t* de_zombie_queue(pcb_t* proc){
    if(proc->zombie->size == 0) {
        log_err("No one in the zombie list!!!");
        return NULL;
    }
    dlist_rm_head(proc->zombie);
}

/* If any child is running
 */
int any_child_active(pcb_t *proc){
    log_info("PID(%d) checking %d children's state", proc->pid, proc->children->size);
    dnode_t *node = proc->children->head;
    while(node) {
        pcb_t *child = node->data;
        if(is_proc_active(child)) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}

/* A general function to initialize user proc
 *
 * @return: A pointer to the newly created pcb;
 *          NULL if creation fails
 */
pcb_t *init_user_proc(pcb_t* parent) {
    // Create pcb
    pcb_t *proc = (pcb_t*) malloc(sizeof(pcb_t));
    if(!proc) {
        log_err("Cannot malloc user proc!");
        return NULL;
    }
    bzero(proc, sizeof(pcb_t));
    
    // Create page table
    proc->page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    if(!proc->page_table) {
        log_err("proc->page_table cannot be malloc!");
        return NULL;
    }
    bzero(proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    
    // Create kernel stack page table
    proc->kernel_stack_pages = (pte_t*) malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    if(!proc->kernel_stack_pages) {
        log_err("proc->kernel_stack_pages cannot be malloc!");
        return NULL;
    }
    bzero(proc->kernel_stack_pages, sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
   
    // Init vitals
    proc->init_done = 0;
    proc->parent = (struct y_PBC*)parent;
    proc->children = dlist_init();
    proc->zombie = dlist_init();
    proc->pid = get_next_pid();
    if(parent) {
        dlist_add_tail(parent->children, proc);
    }
    proc->state = READY;
    proc->wait_zombie = 0;
    return proc;
}

void init_init_proc(void) {
    init_proc = init_user_proc(NULL);
    init_proc->kernel_stack_pages[0].valid = _VALID;
    init_proc->kernel_stack_pages[0].prot = PROT_READ|PROT_WRITE;
    init_proc->kernel_stack_pages[0].pfn = 126;
    init_proc->kernel_stack_pages[1].valid = _VALID;
    init_proc->kernel_stack_pages[1].prot = PROT_READ|PROT_WRITE;
    init_proc->kernel_stack_pages[1].pfn = 127;
    init_proc->init_done = 1;
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
    //proc->user_context = *user_context;
    memcpy(&proc->user_context, user_context, sizeof(UserContext));
    proc->mm = user_memory;
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
    memcpy(user_context, &proc->user_context, sizeof(UserContext));
    //*user_context = proc->user_context;
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

/* Round robin schedule 
 *  1. Enqueue the running proc, if idle is running, don't enqueue
 *  2. Dequeue the next proc in ready queue
 *  3. If the process is delaying, decrement the tick and switch to kernel proc
 *
 *  @param user_context: current user context
 */
void round_robin_schedule(UserContext *user_context) {
    // Don't push running_proc into ready quueue if it is a idle proc
    //log_info("Inside round robin");
    log_info("Round robin with queue size %d, when running PID(%d)", ready_queue->size, running_proc->pid);
    if(!ready_queue->size) {
        return;
    }   
    if(running_proc != idle_proc) {
        en_ready_queue(running_proc);
    }
   
    //log_info("Before next schedule");
    next_schedule(user_context);
    //log_info("Round robin DONE with queue size %d, now running PID(%d)", ready_queue->size, running_proc->pid);

}

/* Pick any process in the ready queue to run
 * if ready queue is empty, run the idle process
 *
 * @param user_context: current user context
 */
void next_schedule(UserContext *user_context) {
    pcb_t *next_proc;   
    int pid = running_proc->pid;
    pcb_t *proc = running_proc;
    if(!ready_queue->size) {
        next_proc = idle_proc;
    } else {
        next_proc = de_ready_queue();
        log_info("De ready_queue get PID(%d)", next_proc->pid);
    }
  
    save_user_runtime(running_proc, user_context);
    pick_schedule(user_context, next_proc);
    log_info("next_schedule done with queue size %d, now running PID(%d) pc(%p) sp(%p)", 
                ready_queue->size, 
                running_proc->pid, 
                running_proc->user_context.pc,
                running_proc->user_context.sp
                );
}

/* schedule a specific process
 */
void pick_schedule(UserContext *user_context, pcb_t *next_proc) {
    context_switch_to(next_proc, user_context);
    restore_user_runtime(running_proc, user_context);
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
        log_err("Stack too low. new addr %p, while user brk high %p. Red zone err!!!", new_addr, proc->mm.brk_high);
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

/* Used for special processes that is not forked to init kernel context,
 * meaning this process is manually created by Yalnix
 */
void init_process_kernel(pcb_t *proc) {
    int rc = 0;
    
    rc = KernelContextSwitch(&init_newbie_kernel, proc, proc);
    if(rc) {
        log_err("Failed to execute magic function!");
        Halt();
    }
    log_info("Init PID(%d) kernel stack done", proc->pid);
}

/* A kernel magic function that is only used for getting kernel context for newbie
 */
KernelContext *init_newbie_kernel(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb){
    pcb_t *next_proc = (pcb_t *) _next_pcb;
    log_info("First time to init PID(%d) kernel stack!", next_proc->pid);
   
    if(next_proc->kernel_stack_pages == NULL) {
        log_err("Init kernel stack fail, pcb->kernel_stack_pages not malloc yet");
        Halt();
    }

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
    next_proc->init_done = 1;
    //print_page_table(kernel_page_table, 120, GET_PAGE_NUMBER(VMEM_0_LIMIT));
    //print_page_table(next_proc->kernel_stack_pages, 0, 2);

    log_info("First time to init PID(%d) kernel stack done", next_proc->pid);
    return kernel_context;
}

/* Determine whether a process is active
 */
int is_proc_active(pcb_t *p) {
    return p && p->state != ZOMBIE && p->state != EXIT;
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
    if(is_proc_active(prev_proc)) {
        memcpy(&prev_proc->kernel_context, kernel_context, sizeof(KernelContext));
    }
    running_proc = next_proc;
    running_proc->state = RUN;
    
    if(next_proc->init_done == 0) {
        // If just initialized (like just forked), init the context and kernel stack
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
        next_proc->init_done = 1;
    }

    // Load kernel stack from next processs and flush corresponding TLB
    int addr;
    memcpy(&kernel_page_table[GET_PAGE_NUMBER(KERNEL_STACK_BASE)], 
            next_proc->kernel_stack_pages, 
            sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE );
    //WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
    for(addr = KERNEL_STACK_BASE; addr < KERNEL_STACK_LIMIT; addr += PAGESIZE) {
        WriteRegister(REG_TLB_FLUSH, addr);
    }

    log_info("Magic kernel switch done from PID(%d) to PID(%d)", prev_proc->pid, next_proc->pid);
    *kernel_context = next_proc->kernel_context;
    return kernel_context;
}

