#ifndef _LOCK_H
#define _LOCK_H
struct y_Lock {
    int             id;
    unsigned long   uid; 
    pcb_t           *wait;
};

// pcb_t*      ready;      // Ready queue, global variable, in *.c file
struct y_CVar {
    lock_t          *lock;
    unsigned long   pid;
    pcb_t*          wait;
};

struct y_Sem {
    int       count;
    int       waking;
    queue_t   waitings;
    lock_t    *lock;
};
#endif

