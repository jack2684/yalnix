/* Some kernal functions
 * 
 */

#include "hardware.h"
#define TRUE 1
#define FALSE 0;
#define ERROR -1

PCB *idle_proc;
PCB *cur_proc;

bool is_vm_enabled = false;

int kernel_data_end;
int kernel_data_start;

handler intvec[TRAP_VECTOR_SIZE];

void SetKernelData(void *_Kernel_Data_Start, void *_Kernel_Data_End) {
    // Set basic parameters 
    TracePrintf(0, "Start=%p, End=%p\n", _Kernel_Data_Start, _Kernel_Data_End);
    kernel_data_start = (int) _Kernel_Data_Start;
    kernel_data_end = (int) _Kernel_Data_End);
}

int KernelStart(char* cmd_args[],  unsigned int pmem_size, UserContext* uctxt ) {
    TracePrintf(0, "Start the kernel\n");
    
    // Initialize vector table mapping from interrupt/exception/trap to a handler fun
    //TrapsInit();
    int TRAP_HANDLER_NUM;
    for(TRAP_HANDLER_NUM = 0; TRAP_HANDLER_NUM < TRAP_VECTOR_SIZE; TRAP_HANDLER_NUM++){
        intvec[TRAP_HANDLER_NUM] = (void *)Trap_Not_Define;
    }
    intvec[TRAP_KERNEL] = trap_kernel_handler;
    intvec[TRAP_CLOCK] = trap_clock_handler;
    intvec[TRAP_ILLEGAL] = trap_illegal_handler;
    intvec[TRAP_MEMORY] = trap_memory_handler;
    intvec[TRAP_MATH] = trap_math_handler;
    intvec[TRAP_TTY_RECEIVE] = trap_tty_rev_handler;
    intvec[TRAP_TTY_TRANSMIT] = trap_tty_trans_handler;
    intvec[TRAP_DISK] = trap_disk_handler;
    
    // REG_VECTOR_BASE point to vector table
    WriteRegister(REG_VECTOR_BASE, (signed int)&intvec);
    
    // Memory management, linked list of frames of free memory
    ???????


    // Build page tables using REG_PTBR0/1 REG_PTLR0/1
    WriteRegister(REG_PTBR0, (unsigned int)ptr0);
    WriteRegister(REG_PTLR0, (unsigned int)VMEM_0_PNUM);
    WriteRegister(REG_VM_ENABLE, 1);

    // Enable virtual memroy 
    is_vm_enabled = TRUE;

    // Create idle process?
    idle_proc = createPCB(uctxt);
    idle_proc -> state = READY;

    // Load init process 

    // Return
}

/* A template from template.c,
 * may be write it in a separate file
 */
int LoadProgram(char *name, char *args[], proc) {
    //implement is load_program.c
}

//Since there may not be enough space, we need a result to check the allocation
int AllocPage(int count, .....){

}

//deallocate will always success, do not need a result 
void DeallocPage(int count, ....){
    
}

int SetKernelBrk (void *addr) {
    TracePrintf(0, "SetKernelBrk: Current Addr = %p\n", addr);

    int current_addr = (int)addr;

    //check if the new break space is trying to access the stack space
    if(current_addr > KERNEL_STACK_BASE){
        TracePrintf(0, "Kernel Break Warning: Trying to Access Stack Addr = %p\n", current_addr);
        return ERROR;
    } // check if trying to access the text space
    else if(addr <= kernel_data_start){
        TracePrintf(0, "Kernel Break Warning: Trying to Access Text Addr = %p\n", addr);
        return ERROR;
    }
    else{ // before the virual memory is enabled
        if(!is_vm_enabled){
            kernel_data_end = current_addr;
        }
        else{
            int count;
            //Enlarge the space
            if(current_addr >= kernel_data_end){ 
                count = ((int)addr >> PAGESHIFT) - ((kernel_data_end) >> PAGESHIFT);
                //Allocate the page
                //int result = AllocPage(count, ........);
                if(result == ERROR){
                    TracePrintf(0, "Kernel Break Warning: No Enough Memory\n");
                    return ERROR;
                }
            }
            else{ //shrink the space
                count = ((int)addr >> PAGESHIFT) - ((kernel_data_end) >> PAGESHIFT);
                //Deallocate the page
                //DeallocPage(count, .......);
            }
            kernel_data_end = current_addr;
        }
        return 0;
    }
}

int KernelContextSwitch(KCSFunc_t *, void *, void *) {
    return SUCCESS;
}



