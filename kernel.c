/* Some kernal functions
 * 
 */

#include "hardware.h"

int KernelStart(char* cmd_args[],  unsigned int pmem_size, UserContext* uctxt ) {
    // Initialize vector table mapping from interrupt/exception/trap to a handler fun

    // REG_VECTOR_BASE point to vector table

    // Memory management, linked list of frames of free memory

    // Build page tables using REG_PTBR0/1 REG_PTLR0/1

    // Enable virtual memroy 

    // Create idle process?

    // Load init process 

    // Return
}

/* A template from template.c,
 * may be write it in a separate file
 */
int LoadProgram(char *name, char *args[], proc) {
    return SUCCESS;
}

int SetKernelBrk (void *arg) {

}

void SetKernelData(void* arg) {
    // Set basic parameters 
}

int KernelContextSwitch(KCSFunc_t *, void *, void *) {
    return SUCCESS;
}



