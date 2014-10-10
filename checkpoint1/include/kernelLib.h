/* Some Yalnix kernel data structures
 */
#ifndef KERNEL_LIB 
#define KERNEL_LIB
#include "hardware.h"
#include "proc.h"
#include "memory.h"
#include "lock.h"
#include "tty.h"

// Page struct
typedef struct y_page page_t;

// Memory management
typedef struct y_mm mm_t;

typedef struct y_PCB pcb_t;

typedef struct y_Lock lock_t;

typedef struct y_CVar cvar_t;

typedef struct y_Sem sem_t;

typedef struct y_Pipe pipe_t;

typedef struct y_TTY tty_t;

// This is done in hardware.h
// typedef y_PTE {} pte_t

// typedef ReadyQueue

// We have this two in hardware.h
// typedef struct UserContext {} userContext_t
// typedef struct KernelContext {} kernalContext_t

#endif

