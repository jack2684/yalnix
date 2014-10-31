/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _LOCK_H
#define _LOCK_H
#include "kernelLib.h"
#include "standardLib.h"

typedef struct y_Lock {
    int             id;
    unsigned long   uid; 
    pcb_t           *wait;
} lock_t;

// pcb_t*      ready;      // Ready queue, global variable, in *.c file
typedef struct y_CVar {
    lock_t          *lock;
    unsigned long   pid;
    pcb_t*          wait;
} cvar_t;

typedef struct y_Sem {
    int       count;
    int       waking;
    //queue_t   waitings;
    lock_t    *lock;
} sem_t;
#endif

