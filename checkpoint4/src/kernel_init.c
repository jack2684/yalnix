/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
/* Some kernal functions
 */
#include "hardware.h"
#include "memory.h"
#include "traps.h"
#include "list.h"
#include "proc.h"
#include "load.h"
#include "common.h"

trap_handler interrupt_vector[TRAP_VECTOR_SIZE];

void SetKernelData _PARAMS((void *_Kernel_Data_Start, void *_Kernel_Data_End)) {
    // Set kernel vm boundaries, noted that most of the *_high is implicitly represented by other's *_low 
    kernel_memory.text_low     = (unsigned int)VMEM_BASE;
    kernel_memory.data_low     = (unsigned int)_Kernel_Data_Start;
    kernel_memory.brk_low      = (unsigned int)_Kernel_Data_End;
    kernel_memory.brk_high     = (unsigned int)_Kernel_Data_End;
    kernel_memory.stack_low    = (unsigned int)KERNEL_STACK_BASE;
    kernel_memory.swap_addr    = (unsigned int)DOWN_TO_PAGE(KERNEL_STACK_BASE) - PAGESIZE;
}

/* 
 * Initialize trap vector
 */
void init_trap_vector() {
    interrupt_vector[TRAP_KERNEL] = trap_kernel_handler;
    interrupt_vector[TRAP_CLOCK] = trap_clock_handler;
    interrupt_vector[TRAP_ILLEGAL] = trap_illegal_handler;
    interrupt_vector[TRAP_MEMORY] = trap_memory_handler;
    interrupt_vector[TRAP_MATH] = trap_math_handler;
    interrupt_vector[TRAP_TTY_RECEIVE] = trap_tty_receive_handler;
    interrupt_vector[TRAP_TTY_TRANSMIT] = trap_tty_transmit_handler;
    interrupt_vector[TRAP_DISK] = trap_disk_handler;
}

/*
 *  Kernel page table must be delicate,
 *  perfectly mapping to physical memory,
 *  so I set all the manually here, 
 *  in order to aligned with phyical memory.
 */
void write_page_table(pte_t *page_table, uint32 idx_start, uint32 idx_end, int isValid, int prot) {
    int i;
    for(i = idx_start; i < idx_end; i++) {
        // _debug("Writing idx %d with pte at %p\n", i, page_table + i);
        page_table[i].valid = isValid;
        page_table[i].prot = prot;
        page_table[i].pfn = i;
    }
}

/* This function make perfect mapping from kernel virtual memory phyical memory 
 */
void init_kernel_page_table() {
    kernel_page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_0_SIZE));
    
    // For text segment mapping
    log_info("Text Start=%p, End=%p", kernel_memory.text_low, kernel_memory.data_low);
    write_page_table(kernel_page_table, 
                    GET_PAGE_FLOOR_NUMBER(kernel_memory.text_low), 
                    GET_PAGE_CEILING_NUMBER(kernel_memory.data_low), 
                    _VALID, PROT_READ | PROT_EXEC);
    
    // For data and heap segment mapping
    log_info("heap low %p and high %p", kernel_memory.brk_low, kernel_memory.brk_high);
    log_info("Data Start=%p, End=%p", kernel_memory.data_low, kernel_memory.brk_high);
    write_page_table(kernel_page_table,
                    GET_PAGE_FLOOR_NUMBER(kernel_memory.data_low), 
                    GET_PAGE_CEILING_NUMBER(kernel_memory.brk_low), 
                    _VALID, PROT_READ | PROT_WRITE);
    
    // For stack segment mapping, noted that stack space is reserved even if it is not used
    log_info("Stack Start=%p, End=%p", KERNEL_STACK_BASE, KERNEL_STACK_LIMIT);
    write_page_table(kernel_page_table,
                    GET_PAGE_NUMBER(KERNEL_STACK_BASE), 
                    GET_PAGE_NUMBER(KERNEL_STACK_LIMIT), 
                    _VALID, PROT_READ | PROT_WRITE);
    
    // Add free pages between heap and stack
    int i;
    int start_pfn = GET_PAGE_NUMBER(UP_TO_PAGE(kernel_memory.brk_high));
    int end_pfn = GET_PAGE_NUMBER(DOWN_TO_PAGE(KERNEL_STACK_BASE));
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
    // Add free pages above kernel space
    start_pfn = GET_PAGE_NUMBER(UP_TO_PAGE(VMEM_0_LIMIT));
    end_pfn = GET_PAGE_NUMBER(DOWN_TO_PAGE(PMEM_BASE)) + total_page_number;
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
}

/*
 * A dummy user page table for the init process
 */
void *init_user_page_table() {
    user_page_table = (void*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
    write_page_table(user_page_table,
                    0, GET_PAGE_NUMBER(VMEM_REGION_SIZE),
                    _INVALID, 0);
    log_info("Init user page done");
}

void DoIdle(void) {
    while(1) {
        TracePrintf(1, "DoIdle\n");
        Pause();
    }
}

void Cooking(pte_t *user_page_table, UserContext* uctxt) {
    map_page_to_frame(user_page_table, 0, 1, PROT_READ | PROT_WRITE);

    uctxt->pc = &DoIdle;
    uctxt->sp = (void*)(VMEM_1_BASE + PAGESIZE - 4); 
}

void KernelStart _PARAMS((char* cmd_args[],  unsigned int pmem_size, UserContext* uctxt )) {
    TracePrintf(0, "Start the kernel\n");
    
    // Initialize vector table mapping from interrupt/exception/trap to a handler fun
    init_trap_vector();

    // REG_VECTOR_BASE point to vector table
    WriteRegister(REG_VECTOR_BASE, (uint32)interrupt_vector);
    
    // Memory management, linked list of frames of free memory
    available_frames = list_init();
    total_page_number = GET_PAGE_NUMBER(pmem_size);
    init_user_page_table();   
    init_kernel_page_table();           
    if(!kernel_page_table || !user_page_table) {
        log_err("Cannot allocate memory for page tables.\n");
        return;
    }

    // Build page tables using REG_PTBR0/1 REG_PTLR0/1 
    WriteRegister(REG_PTBR0, (uint32)kernel_page_table);
    WriteRegister(REG_PTLR0, GET_PAGE_NUMBER(VMEM_0_SIZE));
    //WriteRegister(REG_PTBR1, (uint32)user_page_table); 
    //WriteRegister(REG_PTLR1, GET_PAGE_NUMBER(VMEM_1_SIZE));

    // Enable virtual memroy 
    log_info("Init VM");
    WriteRegister(REG_VM_ENABLE, _ENABLE);
    
    // Init pcb and idle process
    //char* tmp[] = {NULL};
    log_info("Init pcb basic");
    init_processes();
    
    // Create the very first process
    // Noted that, we run init program, and load 
    // user specified program latter in init program 
    // using Exec()
    log_info("Init init");
    init_init_proc();
    LoadProgram("src/init", cmd_args, init_proc);
    *uctxt = init_proc->user_context;
    log_info("Get the first context");
    save_user_runtime(init_proc, uctxt);
    log_info("Saved the first runtime");
    //init_process_kernel(init_proc);
    running_proc = init_proc;
    log_info("Load program PID(%d) done pc(%p) sp(%p)", init_proc->pid, uctxt->pc, uctxt->sp);
    log_info("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");

    init_process_kernel(idle_proc);
    restore_user_runtime(running_proc, uctxt);
    log_info("Leave kernel start with pc %p sp %p", uctxt->pc, uctxt->sp);
    return;
}

/*
 *  Not necessary in checkpoint 2
 */ 
int SetKernelBrk _PARAMS((void *addr)) {
    int rc;
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bound = GET_PAGE_NUMBER(new_addr);
    uint32 current_page_bound = GET_PAGE_NUMBER(kernel_memory.brk_high);
    log_info("SetKernelBrk current brk %p and new addr %p", kernel_memory.brk_high, new_addr);
    
    // Boudaries check 
    if(new_addr > kernel_memory.swap_addr - PAGESIZE) {
        log_err("Kernel Break: Trying to Access Stack Addr = %p\n", new_addr);
        return _FAILURE;
    } // Check if trying to access below brk base line
    else if(new_addr < kernel_memory.brk_low) {
        log_err("Kernel Break: Trying to Access Data Addr = %p\n", addr);
        return _FAILURE;
    }
    // Before the virual memory is enabled
    if(!ReadRegister(REG_VM_ENABLE)) {
        log_info("SetKernelBrk current brk directly to %p Done", kernel_memory.brk_high);
        kernel_memory.brk_high = new_addr;
        return _SUCCESS;
    } 

    // Modify the brk 
    if(new_addr > kernel_memory.brk_high) { 
        rc = map_page_to_frame(kernel_page_table, 
                            current_page_bound, 
                            new_page_bound, 
                            PROT_READ | PROT_WRITE);
        if(rc) {
            log_err("Kernel Break: Not enough phycial memory\n");
            return _FAILURE;
        }
    } else if (new_addr < kernel_memory.brk_high) {
        rc = unmap_page_to_frame(kernel_page_table,
                                new_page_bound, 
                                current_page_bound);
        if(rc) {
            log_err("Kernel Break Warning: Not able to release pages\n");
            return _FAILURE;
        }
    }
    kernel_memory.brk_high = new_addr;
    log_info("SetKernelBrk current brk to %p Done", kernel_memory.brk_high);
    return _SUCCESS;
}


