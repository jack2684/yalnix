#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "memory.h"
#include "dlist.h"


enum proc_stat {
    RUN,
    READY,
    WAIT,
    ZOMBIE,
    EXIT,
};

typedef struct y_PCB {
    // State
    enum proc_stat  state;
    long            priority;
    vm_t            *mm;            // Memory management
    int             exit_code;
    int             exit_signail;
    UserContext     user_context;
    KernelContext   kernel_context;
    pte_t           *page_table;
    
    // Identity
    int             pid;
    int             ppid;
    unsigned long   uid;
    unsigned long   gid;

    // Connected
    dnode_t         *list_node;          // For high performance list operation
    struct y_PBC    *children;
    struct y_PBC    *wait;
} pcb_t;

extern pcb_t   *kernel_proc;          // A kernel proc
extern pcb_t   *user_proc;          // A user proc
extern pcb_t   *running_proc;      // Current running proc
extern dlist_t  *ready_queue;   
extern dlist_t  *wait_queue;

void init_processes(void);
void DoDoIdle(void);
void init_kernel_proc(void);
void init_user_proc(void);
int en_ready_queue(pcb_t *proc);
pcb_t* de_ready_queue(pcb_t *proc);

#endif

