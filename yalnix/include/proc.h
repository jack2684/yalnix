#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "memory.h"
#include "dlist.h"

#define MAX_PROC 256

enum proc_stat {
    RUN,
    READY,
    WAIT,
    ZOMBIE,
    EXIT,
    PAUSE,
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
    pte_t*          kernel_stack_pages;// [KERNEL_STACK_MAXSIZE / PAGESIZE];
    int             ticks;
    
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
extern pcb_t   *idle_proc;          // A user proc
extern pcb_t   *running_proc;      // Current running proc
extern dlist_t  *ready_queue;   
extern dlist_t  *wait_queue;

void init_processes(void);
void DoDoIdle(void);
void init_kernel_proc(void);
pcb_t* init_user_proc(void);
int en_ready_queue(pcb_t *proc);
void save_and_en_ready_queue(pcb_t *proc, UserContext *user_context);
pcb_t* de_ready_queue_and_run(UserContext *user_context);
pcb_t* de_ready_queue();
pcb_t* rm_ready_queue(pcb_t *proc);
void round_robin_schedule(UserContext *user_context);
void next_schedule(UserContext *user_context);
void save_user_runtime(pcb_t *proc, UserContext *user_context);
void restore_user_runtime(pcb_t *proc, UserContext *user_context);
int user_stack_resize(pcb_t *proc, uint32 addr);

KernelContext *kernel_context_switch(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb);
void switch_to_process(pcb_t * next_proc, UserContext * user_context);

#endif

