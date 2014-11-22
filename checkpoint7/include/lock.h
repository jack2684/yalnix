/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _LOCK_H
#define _LOCK_H
#include "inthashmap.h"
#include "dlist.h"
#include "proc.h"

#define MAX_LOCKS 104

typedef struct y_Lock {
    int             id;
    dlist_t         *waits;
    pcb_t           *owner;
} lock_t;

// pcb_t*      ready;      // Ready queue, global variable, in *.c file
typedef lock_t cvar_t;
//typedef struct y_CVar {
//    int id;
//    pcb_t*          waits;
//    pcb_t           *owner;
//} cvar_t;

/*typedef struct y_Sem {
    int       count;
    int       waking;
    //queue_t   waitings;
    lock_t    *lock;
} sem_t;*/

//extern hashmap_t *lock_idp;
//extern dlist_t *lock_id_list;
//extern hashmap_t *cvar_idp;
//extern dlist_t *cvar_id_list;

// Mutex lock
int lock_init(int *id);
int lock_acquire(lock_t *lock, UserContext *user_context);
int lock_release(lock_t *lock);
int free_lock(lock_t *lock);

// Condition variable lock
int cvar_init(int *id);
int cvar_wait(cvar_t* cvar, lock_t *lock, UserContext *user_context);
int cvar_signal(cvar_t* cvar);
int cvar_broadcast(cvar_t* cvar);
int free_cvar(cvar_t *cvar);

// Helper functions

#endif

