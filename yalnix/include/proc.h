#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "memory.h"
#include "list.h"

enum proc_stat {
    RUN,
    READY,
    WAIT,
    ZOMBIE,
    EXIT,
}

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
    node_t          *list_node;          // For high performance list operation
    struct y_PBC    *children;
    struct y_PBC    *wait;
} pcb_t;
#endif

