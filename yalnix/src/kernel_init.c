/* Some kernal functions
 */
#include "hardware.h"
#include "memory.h"
#include "traps.h"
#include "list.h"
#include "common.h"

trap_handler interrupt_vector[TRAP_VECTOR_SIZE];

void SetKernelData(void *_Kernel_Data_Start, void *_Kernel_Data_End) {
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
void write_kernel_pte(uint32 addr_low, uint32 addr_high, int isValid, int prot) {
    int i;
    for(i = GET_PAGE_NUMBER(addr_low); i < GET_PAGE_NUMBER(addr_high); i++) {
        kernel_page_table[i].valid = isValid;
        kernel_page_table[i].prot = prot;
        kernel_page_table[i].pfn = i;
    }
}

/* This function make perfect mapping from kernel virtual memory phyical memory 
 */
void init_kernel_page_table() {
    kernel_page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_0_SIZE));
    
    // For text segment mapping
    write_kernel_pte(kernel_memory.text_low, kernel_memory.data_low
            , _VALID, PROT_READ | PROT_EXEC);
    
    // For data segment mapping
    write_kernel_pte(kernel_memory.data_low, kernel_memory.brk_low
            , _VALID, PROT_READ | PROT_WRITE);
    
    // For stack segment mapping, noted that stack space is reserved even if it is not used
    write_kernel_pte(KERNEL_STACK_BASE, KERNEL_STACK_LIMIT
            , _VALID, PROT_READ | PROT_WRITE);
    
    int i;
    // Add free pages between heap and stack
    int start_pfn = GET_PAGE_NUMBER(kernel_memory.brk_high);
    int end_pfn = GET_PAGE_NUMBER(KERNEL_STACK_BASE);
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
    // Add free pages above kernel space
    start_pfn = GET_PAGE_NUMBER(kernel_memory.stack_high);
    end_pfn = GET_PAGE_NUMBER(UP_TO_PAGE(PMEM_BASE)) + PAGE_SIZE;
    for(i = start_pfn; i < end_pfn; i++) {
        add_tail_available_frame(i);
    }
}

/*
 * A dummy user page table for the init process
 */
pte_t *init_user_page_table() {
    return (void*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
}

void DoIdle(void) {
    while(1) {
        TracePrintf(1,"DoIdle\n");
        pause();
    }
}

void KernelStart(char* cmd_args[],  unsigned int pmem_size, UserContext* uctxt ) {
    TracePrintf(0, "Start the kernel\n");
    pte_t *user_page_table;
    
    // Initialize vector table mapping from interrupt/exception/trap to a handler fun
    init_trap_vector();

    // REG_VECTOR_BASE point to vector table
    WriteRegister(REG_VECTOR_BASE, (uint32)interrupt_vector);
    
    // Memory management, linked list of frames of free memory
    available_frames = list_init();
    PAGE_SIZE = GET_PAGE_NUMBER(pmem_size);
    init_kernel_page_table();
    user_page_table = init_user_page_table();
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
    WriteRegister(REG_VM_ENABLE, _ENABLE);

    // Create idle kernel proc, maybe
    DoIdle();

    // Load init process (in checkpoint 3)
    
    return;
}

/*
 *  Not necessary in checkpoint 2
 */ 
int SetKernelBrk (void *addr) {
    TracePrintf(0, "SetKernelBrk: Current Addr = %p\n", addr);
    int page_cnt, rc;
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bound = UP_TO_PAGE(new_addr);
    uint32 current_page_bound = UP_TO_PAGE(kernel_memory.brk_high);
    
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
        kernel_memory.brk_high = new_addr;
        return _SUCCESS;
    } 
    
    // Modify the brk 
    if(new_addr > kernel_memory.brk_high) { 
        page_cnt = GET_PAGE_NUMBER(new_page_bound) - GET_PAGE_NUMBER(current_page_bound);
        rc = map_page_to_frame(kernel_page_table, 
                            current_page_bound, 
                            page_cnt, 
                            PROT_READ | PROT_WRITE);
        if(rc) {
            TracePrintf(0, "Kernel Break Warning: Not enough phycial memory\n");
            return _FAILURE;
        }
    } else if (new_page_bound < kernel_memory.brk_high) {
        page_cnt = GET_PAGE_NUMBER(new_page_bound) - GET_PAGE_NUMBER(current_page_bound);
        rc = unmap_page_to_frame(kernel_page_table,
                                new_page_bound, 
                                page_cnt);
        if(rc) {
            TracePrintf(0, "Kernel Break Warning: Not able to release pages\n");
            return _FAILURE;
        }
    }
    kernel_memory.brk_high = new_addr;
    return _SUCCESS;
}

// int KernelContextSwitch(KCSFunc_t *, void *, void *) {
//     return _SUCCESS;
// }



