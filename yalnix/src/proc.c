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
    kernel_proc->kernel_stack_pages = NULL;
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
    user_proc->kernel_stack_pages = NULL;
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
    pcb_t *about_to_run = dlist_rm_head(ready_queue);
    if(about_to_run == NULL) {
        log_info("Ready queue is empty, suck it.");
        return NULL;
    }
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
    pcb_t *next_proc = de_ready_queue();
    memcpy(user_context, &(next_proc->user_context), sizeof(UserContext));
    memcpy(user_page_table, next_proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    memcpy(&user_memory, &(next_proc->mm), sizeof(vm_t));
    next_proc->kernel_stack_pages = (pte_t*)malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    if(next_proc->kernel_stack_pages) {
        log_err("Cannot malloc next_proc->kernel_stack_pages");
    }
    log_info("de_ready_queue_and_run next_proc->kernel_stack_pages addr %p with PID %d", next_proc->kernel_stack_pages, next_proc->pid);
    memcpy(next_proc->kernel_stack_pages, &kernel_page_table[GET_PAGE_FLOOR_NUMBER(KERNEL_STACK_BASE)], sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
    log_info("de_ready_queue_and_run next_proc->kernel_stack_pages addr %p with PID %d", next_proc->kernel_stack_pages, next_proc->pid);
    running_proc = next_proc;
    return next_proc;
}

void ticking_down(){
    if(!ready_queue->size) {
        return;
    }
    dnode_t* node = ready_queue->head;
    while(node) {
        pcb_t* proc = (pcb_t*)node->data;
        proc->ticks = proc->ticks == 0 ? 0 : proc->ticks -1;
        node = node->next;
    }
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
  
    log_info("Before save next_proc->kernel_stack_pages addr %p with PID %d", running_proc->kernel_stack_pages, running_proc->pid);
    //if(running_proc != kernel_proc) {
        
    //    stall_running_and_en_ready_queue(user_context);
    //}
    safe_and_en_ready_queue(running_proc, user_context);
    pcb_t *next_proc;
    next_proc = de_ready_queue();
    log_info("next_proc->kernel_stack_pages addr %p with PID %d", next_proc->kernel_stack_pages, next_proc->pid);
    if(next_proc->ticks > 0) {
        en_ready_queue(next_proc);
    } else {
        log_info("PID %d get back to live!", next_proc->pid);
        
        memcpy(user_context, &(next_proc->user_context), sizeof(UserContext));
        memcpy(user_page_table, next_proc->page_table, sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
        memcpy(&user_memory, &(next_proc->mm), sizeof(vm_t));
        switch_to_process(next_proc, user_context);
        running_proc = next_proc;
    }
}

void switch_to_process(pcb_t *next_proc, UserContext *user_context) {
    log_info("Before set context");
    running_proc->user_context = *user_context;
    *user_context = next_proc->user_context;

    log_info("Before flush");
    WriteRegister(REG_PTBR1, (unsigned int)user_page_table);
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    log_info("Before set running");
    pcb_t *prev_proc = running_proc;
    running_proc = next_proc;

    // Using the magic function
    log_info("Before magic");
    int rc = KernelContextSwitch(&kernel_context_switch, prev_proc, next_proc);
    if (_SUCCESS == rc) {
        log_info("Succesfully switched kernel context to PID %d!", running_proc->pid);
    } else {
        log_err("Failed to save kernel context!");
        Halt();
    }
    log_info("Done magic");
    
    // Load the new user context
    *user_context = running_proc->user_context;
    log_info("Done context swtiching");
}

KernelContext *kernel_context_switch(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb)
{
    

    pcb_t *prev_proc = (pcb_t *) _prev_pcb;
    pcb_t *next_proc = (pcb_t *) _next_pcb;
    log_info("Start Save And Switch Kernel Context from PID(%d) to PID(%d)", prev_proc->pid, next_proc->pid);

    if(prev_proc != NULL) {
        memcpy(&prev_proc->kernel_context, kernel_context, sizeof(KernelContext));
    }
    
    log_info("Start copy stack with #page");

    // Copy kernel stack 
    if(!next_proc->kernel_stack_pages) {
        
        next_proc->kernel_context = *kernel_context; 
        next_proc->kernel_stack_pages = (pte_t*)malloc(sizeof(pte_t) * KERNEL_STACK_MAXSIZE / PAGESIZE);
        if(next_proc->kernel_stack_pages) {
            log_err("Cannot malloc next_proc->kernel_stack_pages");
        }
        log_info("next_proc->kernel_stack_pages addr %p with PID %d", next_proc->kernel_stack_pages, next_proc->pid);
        map_page_to_frame(next_proc->kernel_stack_pages, 0, KERNEL_STACK_MAXSIZE / PAGESIZE, PROT_READ | PROT_WRITE);
    }
    log_info("About to restore kernel stack from addr %p", next_proc->kernel_stack_pages);
    
    memcpy(&kernel_page_table[GET_PAGE_FLOOR_NUMBER(KERNEL_STACK_BASE)], next_proc->kernel_stack_pages, sizeof(next_proc->kernel_stack_pages));
    log_info("Start flushing reg 1");
    WriteRegister(REG_PTBR1, (unsigned int)next_proc -> page_table);
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);
    log_info("Start flushing k stack");
    int addr;
    for(addr = KERNEL_STACK_BASE; addr < KERNEL_STACK_LIMIT; addr += PAGESIZE) {
        WriteRegister(REG_TLB_FLUSH, addr);
    }

    log_info("About to return kernel_context addr %p", &next_proc->kernel_context);
    return &next_proc->kernel_context;
}

