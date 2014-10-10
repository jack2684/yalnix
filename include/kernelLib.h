/* Some Yalnix kernel data structures
 */
#ifndef KERNEL_LIB 
#define KERNEL_LIB
#include "hardware.h"

// Page struct
typedef struct y_page {
    unsigned long   base;
    unsigned long   offset;
    struct y_page   *next;
    struct y_page   *prev;
} page_t;

// Memory management
typedef struct y_mm {
//   struct y_VMA     *vma_list;                  // List of VMAs ??
   unsigned long    code_start, code_end;
   unsigned long    data_start, data_end;      
   unsigned long    env_start, env_end;         
   unsigned long    stack_start, stack_end;     
   unsigned long    size;                       // Size of a process memory space
} mm_t;

typedef struct y_PCB {
    // State
    int             state;
    long            priority;
    mm_t            *mm;            // Memory management
    int             exit_code;
    int             exit_signail;
    UserContext     user_context;
    KernelContext   kernel_context;
    
    // Identity
    int             pid;
    int             ppid;
    unsigned long   uid;
    unsigned long   gid;

    // Connected
    struct y_PCB    *next_task;
    struct y_PCB    *prev_task;
    struct y_PBC    *children;
    struct y_PBC    *wait;
} pcb_t;

typedef struct y_Lock {
    int id;
} lock_t;

typedef struct y_Sem {
    int     count;
    int     waking;
    queue_t waitings;
} sem_t;

typedef struct y_Pipe {
    char*   buff;
    int     max_len;
    int     len;
    int     readIdx;
    int     writeIdx;
} pipe_t;

typedef struct y_TTY {} tty_t;

// This is done in hardware.h
// typedef y_PTE {} pte_t

// typedef ReadyQueue

// We have this two in hardware.h
// typedef struct UserContext {} userContext_t
// typedef struct KernelContext {} kernalContext_t

#endif

