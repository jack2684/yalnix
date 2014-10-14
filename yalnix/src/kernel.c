/* Some kernal functions
 * 
 */
#include "hardware.h"
#include "memory.h"
#include "traps.h"
#include "common.h"

trap_handler interrupt_vector[TRAP_VECTOR_SIZE];

void SetKernelData(void *_Kernel_Data_Start, void *_Kernel_Data_End) {
    // Set kernel vm boundaries, noted that most of the *_high is implicitly represented by other's *_low 
    TracePrintf(0, "Start=%p, End=%p\n", _Kernel_Data_Start, _Kernel_Data_End);
    kernel_memory->text_low     = (unsigned long)PMEM_BASE;
    kernel_memory->data_low     = (unsigned long)_Kernel_Data_Start;
    kernel_memory->brk_low      = (unsigned long)_Kernel_Data_End;
    // kernel_memory->stack_low
    // @TODO: calculate the stack size
    kernel_memory->stack_high   = (unsigned long)KERNEL_STACK_LIMIT;
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
    interrupt_vector[TRAP_TTY_RECEIVE] = trap_tty_rev_handler;
    interrupt_vector[TRAP_TTY_TRANSMIT] = trap_tty_trans_handler;
    interrupt_vector[TRAP_DISK] = trap_disk_handler;
}

/*
 *  Kernel page table must be delicate,
 *  perfectly mapping to physical memory,
 *  so I set all the manually here, 
 *  in order to aligned with phyical memory.
 */
void write_kernel_pte(uint32 addr_low, uint32 addr_high, int isValid, int prot) {
    int i = GET_PAGE_NUMBER(addr_low);
    for(; i < GET_PAGE_NUMBER(addr_high); i++) {
        kernel_page_table[i].valid = isValid;
        kernel_page_table[i].prot = prot;
        kernel_page_table[i].pfn = i;
    }
}

void init_kernel_page_table() {
    kernel_page_table = (pte_t*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_0_SIZE));
    
    // For text segment mapping
    write_kernel_pte(kernel_memory->code_low, kernel_memory->code_high
            , _VALID, PROT_READ | PROT_EXEC);
    
    // For data segment mapping
    write_kernel_pte(kernel_memory->brk_low, kernel_memory->brk_high
            , _VALID, PROT_READ | PROT_WRITE);
    
    // For stack segment mapping
    write_kernel_pte(kernel_memory->stack_low, kernel_memory->stack_high
            , _VALID, PROT_READ | PROT_WRITE);
    
   // @TODO: add all those free frames using add_tail_available_frame 
}

/*
 * A dummy user page table for the init process
 */
pte_t *init_user_page_table() {
    return (void*) malloc(sizeof(pte_t) * GET_PAGE_NUMBER(VMEM_1_SIZE));
}

int KernelStart(char* cmd_args[],  uint32 pmem_size, UserContext* uctxt ) {
    TracePrintf(0, "Start the kernel\n");
    pte *user_page_table;
    
    // Initialize vector table mapping from interrupt/exception/trap to a handler fun
    init_trap_vector();

    // REG_VECTOR_BASE point to vector table
    WriteRegister(REG_VECTOR_BASE, (uint32)&interrupt_vector);
    
    // Memory management, linked list of frames of free memory
    init_kernel_page_table();
    user_page_table = init_user_page_table();
    if(!kernel_page_table || !user_page_table) {
        _debug("Cannot allocate memory for page tables.\n");
        return FAILURE;
    }

    // Build page tables using REG_PTBR0/1 REG_PTLR0/1 
    WriteRegister(REG_PTBR0, (uint32)kernel_page_table);
    WriteRegister(REG_PTLR0, (uint32)VMEM_0_PNUM);
    WriteRegister(REG_PTBR1, (uint32)user_page_table);
    WriteRegister(REG_PTLR1, (uint32)VMEM_1_PNUM);

    // Enable virtual memroy 
    WriteRegister(REG_VM_ENABLE, ENABLE);

    // Create idle kernel proc, maybe
    // ...

    // Load init process (in checkpoint 3)
    
    return _SUCCESS;
}

/*
 *  Not necessary in checkpoint 2
 */ 
int SetKernelBrk (void *addr) {
    TracePrintf(0, "SetKernelBrk: Current Addr = %p\n", addr);
    int page_cnt, rc;
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bond = UP_TO_PAGE(current_addr);
    uint32 current_page_bond = UP_TO_PAGE(kernel_memory->brk_high);
    
    // Boudaries check
    if(current_addr > kernel_memory->stack_low) {
        TracePrintf(0, "Kernel Break Warning: Trying to Access Stack Addr = %p\n", current_addr);
        return _FAILURE;
    } // Check if trying to access below brk base line
    else if(addr < kernel_memory->brk_low) {
        TracePrintf(0, "Kernel Break Warning: Trying to Access Text Addr = %p\n", addr);
        return _FAILURE;
    }
    // Before the virual memory is enabled
    if(!ReadRegister(REG_VM_ENABLE)) {
        kernel_memroy->brk_low = current_addr;
    } 
    
    // Modify the brk 
    if(current_addr > kernel_memory->brk_high) { 
        page_cnt = GET_PAGE_NUMBER(new_page_bond) - GET_PAGE_NUMBER(current_page_bond);
        rc = map_page_to_frame(kernel_page_table, 
                            current_page_bond, 
                            page_cnt, 
                            PROT_READ | PROT_WRITE);
        if(rc) {
            TracePrintf(0, "Kernel Break Warning: Not enough phycial memory\n");
            return _FAILURE;
        }
    } else if (new_page_bond < kernel_data_high) {
        count = ((int)addr >> PAGESHIFT) - ((kernel_data_high) >> PAGESHIFT);
        rc = unmap_page_to_frame(kernel_page_table, 
                                current_page_bond, 
                                page_cnt);
        if(rc) {
            cePrintf(0, "Kernel Break Warning: Not able to release pages\n");
            return _FAILURE
        }
    }
    kernel_memory->brk_high = current_addr;
    return _SUCCESS;
}

int KernelContextSwitch(KCSFunc_t *, void *, void *) {
    return _SUCCESS;
}



