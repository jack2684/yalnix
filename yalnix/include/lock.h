/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _LOCK_H
#define _LOCK_H
#include "inthashmap.h"
#include "dlist.h"

#define MAX_LOCKS 104

typedef struct y_Lock {
    int             id;
    dlist_t         *waits;
    pcb_t           *owner;
} lock_t;

// pcb_t*      ready;      // Ready queue, global variable, in *.c file
typedef struct y_CVar {
    lock_t          *lock;
    pcb_t*          waits;
} cvar_t;

typedef struct y_Sem {
    int       count;
    int       waking;
    //queue_t   waitings;
    lock_t    *lock;
} sem_t;

extern hashmap_t *lock_idp;
extern dlist_t *lock_id_list;

lock_t *lock_init();
int locK_acquire(lock_t *lock, UserContext *user_context);
int locK_release(lock_t *lock, UserContext *user_context);
int free_lock();

// Helper functions
int get_next_lock_id();

#endif

