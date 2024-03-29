#ifndef PROC_H
#define PROC_H
struct y_PCB {
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
};
#endif

