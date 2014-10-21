/* Some kernal functions
 */
#include "hardware.h"
#include "memory.h"
#include "traps.h"
#include "list.h"
#include "common.h"

trap_handler interrupt_vector[TRAP_VECTOR_SIZE];

void SetKernelData _PARAMS((void *_Kernel_Data_Start, void *_Kernel_Data_End)) {
    // Set kernel vm boundaries, noted that most of the *_high is implicitly represented by other's *_low 
    TracePrintf(0, "Start=%p, End=%p\n", _Kernel_Data_Start, _Kernel_Data_End);
    kernel_memory.text_low     = (unsigned int)VMEM_BASE;
    kernel_memory.data_low     = (unsigned int)_Kernel_Data_Start;
    kernel_memory.brk_low      = (unsigned int)_Kernel_Data_End;
    kernel_memory.brk_high     = (unsigned int)_Kernel_Data_End;
    // kernel_memory.stack_low    = (unsigned long)KERNEL_STACK_LIMIT - WORD_LEN / 8;
    kernel_memory.stack_low    = (unsigned int)KERNEL_STACK_BASE;
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
    TracePrintf(0, "Text Start=%p, End=%p\n", kernel_memory.text_low, kernel_memory.data_low);
    write_page_table(kernel_page_table, 
                    GET_PAGE_NUMBER(kernel_memory.text_low), 
                    GET_PAGE_NUMBER(kernel_memory.data_low), 
                    _VALID, PROT_READ | PROT_EXEC);
    
    // For data segment mapping
    TracePrintf(0, "Data Start=%p, End=%p\n", kernel_memory.data_low, kernel_memory.brk_low);
    write_page_table(kernel_page_table,
                    GET_PAGE_NUMBER(kernel_memory.data_low), 
                    GET_PAGE_NUMBER(kernel_memory.brk_low), 
                    _VALID, PROT_READ | PROT_WRITE);
    
    // For stack segment mapping, noted that stack space is reserved even if it is not used
    TracePrintf(0, "Stack Start=%p, End=%p\n", KERNEL_STACK_BASE, KERNEL_STACK_LIMIT);
    write_page_table(kernel_page_table,
                    GET_PAGE_NUMBER(KERNEL_STACK_BASE), 
                    GET_PAGE_NUMBER(KERNEL_STACK_LIMIT), 
                    _VALID, PROT_READ | PROT_WRITE);
    
    int i;
    // Add free pages between heap and stack
    int start_pfn = GET_PAGE_NUMBER(UP_TO_PAGE(kernel_memory.brk_high));
    int end_pfn = GET_PAGE_NUMBER(DOWN_TO_PAGE(KERNEL_STACK_BASE));
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
    // Add free pages above kernel space
    start_pfn = GET_PAGE_NUMBER(UP_TO_PAGE(VMEM_0_LIMIT));
    end_pfn = GET_PAGE_NUMBER(DOWN_TO_PAGE(PMEM_BASE)) + PAGE_SIZE;
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
}

/*
 * A dummy user page table for the init process
 */
void *init_user_page_table() {
    user_page_table = (void*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
   // write_page_table(user_page_table,
   //                 0, GET_PAGE_NUMBER(VMEM_1_SIZE),
   //                 _VALID, PROT_READ)
}

void DoIdle(void) {
    while(1) {
        TracePrintf(1, "DoIdle\n");
        pause();
    }
}

void Cooking(pte_t *user_page_table, UserContext* uctxt) {
    // map_page_to_frame(user_page_table, GET_PAGE_NUMBER(VMEM_1_BASE), 1, PROT_READ | PROT_WRITE);
    int i = GET_PAGE_NUMBER(VMEM_1_LIMIT);
    user_page_table[0].valid = _VALID;
    user_page_table[0].prot = PROT_READ | PROT_WRITE;
    user_page_table[0].pfn = frame_get_pfn(list_rm_idx(available_frames, 0));

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
    PAGE_SIZE = GET_PAGE_NUMBER(pmem_size);
    init_user_page_table();   
    init_kernel_page_table();           
    if(!kernel_page_table || !user_page_table) {
        _debug("Cannot allocate memory for page tables.\n");
        return;
    }


    // Build page tables using REG_PTBR0/1 REG_PTLR0/1 
    WriteRegister(REG_PTBR0, (uint32)kernel_page_table);
    WriteRegister(REG_PTLR0, GET_PAGE_NUMBER(VMEM_0_SIZE));
    WriteRegister(REG_PTBR1, (uint32)user_page_table);
    WriteRegister(REG_PTLR1, GET_PAGE_NUMBER(VMEM_1_SIZE));

    // Enable virtual memroy 
    _debug("+-+-+-Init VM+-+-+-\n");
    WriteRegister(REG_VM_ENABLE, _ENABLE);
    int *a;
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);
    a= (int*) malloc(sizeof(int) * 1000);

    // Create idle proc
    Cooking(user_page_table, uctxt);

    // Load init process (in checkpoint 3)
    
    TracePrintf(0, "Leave the kernel\n");
    return;
}

/*
 *  Not necessary in checkpoint 2
 */ 
int SetKernelBrk _PARAMS((void *addr)) {
    TracePrintf(0, "SetKernelBrk Start = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);
    
    int page_cnt, rc;
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bound = GET_PAGE_NUMBER(UP_TO_PAGE(new_addr));
    uint32 current_page_bound = GET_PAGE_NUMBER(UP_TO_PAGE(kernel_memory.brk_high));
    
    // Boudaries check
    if(new_addr > kernel_memory.stack_low) {
        TracePrintf(0, "Kernel Break Warning: Trying to Access Stack Addr = %p\n", new_addr);
        return _FAILURE;
    } // Check if trying to access below brk base line
    else if(new_addr < kernel_memory.brk_low) {
        TracePrintf(0, "Kernel Break Warning: Trying to Access Text Addr = %p\n", addr);
        return _FAILURE;
    }
    // Before the virual memory is enabled
    if(!ReadRegister(REG_VM_ENABLE)) {
        _debug("VM not enabled, current brk: %p, new addr: %p \n", kernel_memory.brk_high, new_addr); 
        kernel_memory.brk_high = new_addr;
        return _SUCCESS;
    } 
    TracePrintf(0, "SetKernelBrk CHECK DONE = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);

    // Modify the brk 
    if(new_addr > kernel_memory.brk_high) { 
        TracePrintf(0, "SetKernelBrk ADD = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);
        page_cnt = new_page_bound - current_page_bound;
        rc = map_page_to_frame(kernel_page_table, 
                            current_page_bound, 
                            page_cnt, 
                            PROT_READ | PROT_WRITE);
        if(rc) {
            TracePrintf(0, "Kernel Break Warning: Not enough phycial memory\n");
            return _FAILURE;
        }
        TracePrintf(0, "SetKernelBrk ADD DONE = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);
    } else if (new_addr < kernel_memory.brk_high) {
        TracePrintf(0, "SetKernelBrk RM = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);
        page_cnt = new_page_bound - current_page_bound;
        rc = unmap_page_to_frame(kernel_page_table,
                                new_page_bound, 
                                page_cnt);
        if(rc) {
            TracePrintf(0, "Kernel Break Warning: Not able to release pages\n");
            return _FAILURE;
        }
    }
    kernel_memory.brk_high = new_addr;
    TracePrintf(0, "SetKernelBrk DONE = %p, End = %p, New Addr = %p\n", kernel_memory.brk_low, kernel_memory.brk_high, addr);
    return _SUCCESS;
}

// int KernelContextSwitch(KCSFunc_t *, void *, void *) {
//     return _SUCCESS;
// }



