/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "yalnix.h"
#include "dlist.h"
#include "aux.h"
#include "memory.h"

#define MAX_PROCS 1024
#define MIN_PROCS 10

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
    int             init_done;
    int             wait_zombie;
    int             child_thread;       // Whether it is a thread process

    // Identity
    int             pid;
    int             ppid;
    unsigned long   uid;
    unsigned long   gid;

    // Relationship with other procs
    dnode_t         *list_node;          // For high performance list operation
    dlist_t         *children;          
    dlist_t         *zombie;             // FIFO list of zombie children
    struct y_PBC    *parent;

    // Utilities
    char            *tty_buf;
    dlist_t         *utils;              // List of utils, includes lock, pipe and cvar
} pcb_t;

extern pcb_t   *init_proc;          
extern pcb_t   *idle_proc;          
extern pcb_t   *running_proc;      // Current running proc
extern dlist_t  *ready_queue;   
extern dlist_t  *wait_queue;
extern dlist_t  *write_queue;
extern dlist_t  *read_queue;
extern dlist_t  *pid_list;

/* Basic prcess functions
 */
void init_processes();
pcb_t* init_user_proc(pcb_t *parent);
pcb_t* init_user_thread(pcb_t *parent);
void init_idle_proc(void);
int en_ready_queue(pcb_t *proc);
int proc_enqueue(dlist_t *queue, pcb_t *proc);
pcb_t *proc_dequeue(dlist_t *queue);
void save_and_en_ready_queue(pcb_t *proc, UserContext *user_context);
pcb_t *de_ready_queue();
pcb_t *rm_ready_queue(pcb_t *proc);
void round_robin_schedule(UserContext *user_context);
void next_schedule(UserContext *user_context);
void pick_schedule(UserContext *user_context, pcb_t *next_proc);
void save_user_runtime(pcb_t *proc, UserContext *user_context);
void restore_user_runtime(pcb_t *proc, UserContext *user_context);
int user_stack_resize(pcb_t *proc, uint32 addr);
int is_proc_active(pcb_t *proc);
int copy_user_runtime(pcb_t *dest_proc, pcb_t *src_proc, UserContext *user_context);
int copy_local_runtime(pcb_t *dest_proc, pcb_t *src_proc, UserContext *user_context);
void init_init_proc(void);
int proc_add_util(pcb_t *proc, int id);

/* Parent and children functions
 */
int free_proc(pcb_t *proc);
int clear_proc(pcb_t *proc);
void tell_children(pcb_t *proc);
void tell_parent(pcb_t *proc);
int en_wait_queue(pcb_t* proc);
pcb_t* rm_wait_queue(pcb_t* proc);
int en_zombie_queue(pcb_t* proc, pcb_t* child);
pcb_t* de_zombie_queue(pcb_t* proc);
int any_child_active(pcb_t *proc);
pcb_t* en_children_queue(pcb_t *proc);

/* Kernel context switch functions
 */
void init_process_kernel(pcb_t *proc);
KernelContext *init_newbie_kernel(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb);
void context_switch_to(pcb_t * next_proc, UserContext * user_context);
KernelContext *kernel_context_switch(KernelContext *kernel_context, void *_prev_pcb, void *_next_pcb);

/* Internal helper funcitons
 */
int get_next_pid();
#endif

