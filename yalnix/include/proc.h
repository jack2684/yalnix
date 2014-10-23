#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "memory.h"
#include "timer.h"
#include "dlist.h"

#define MAX_PROC 256

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
    vm_t            mm;            // Memory management
    int             exit_code;
    int             exit_signail;
    UserContext     user_context;
    KernelContext   kernel_context;
    pte_t*          page_table;
    int             remaining_clock_ticks;
    
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
pcb_t* init_user_proc(void);
int en_ready_queue(pcb_t *proc);
void safe_and_en_ready_queue(pcb_t *proc, UserContext *user_context);
void stall_running_and_en_ready_queue(UserContext *user_context);
pcb_t* de_ready_queue_and_run(UserContext *user_context);
pcb_t* de_ready_queue();
pcb_t* rm_ready_queue(pcb_t *proc);
void round_robin_schedule(UserContext *user_context);

#endif

