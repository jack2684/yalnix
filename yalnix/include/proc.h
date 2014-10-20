#ifndef _PROC_H
#define _PROC_H
#include "kernelLib.h"
#include "standardLib.h"

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
#endif

