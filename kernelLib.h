#ifndef KERNEL_LIB 
#define KERNEL_LIB

/* Some Yalnix kernel data structures
 */

// Memory management
typedef struct y_Mem {

} mm_t;

typedef struct y_PCB {
    // State
    int             state;
    long            priority;
    mm_t            *mm;            // Memory management
    int             exit_code;
    int             exit_signal;
    
    // Identity
    int             pid;
    int             ppid;
    unsigned long   uid;
    unsigned long   gid;

    // Connected
    struct y_PCB*   *next_task;
    struct y_PCB*   *prev_task;
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
    char*   data;
    lock_t  lock;
} pipe_t;

typedef struct y_TTY {} tty_t;

// This is done in hardware.h
// typedef y_PTE {} pet_t

// typedef ReadyQueue

// We have this two in hardware.h
// typedef struct UserContext {} userContext_t
// typedef struct KernelContext {} kernalContext_t

#endif

