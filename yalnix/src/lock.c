/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "lock.h"

hashmap_t *lock_idp = NULL;
dlist_t *lock_id_list = NULL;
hashmap_t *cvar_idp = NULL;
dlist_t *cvar_id_list = NULL;

lock_t *lock_init() {
    if(lock_idp == NULL) {
        lock_idp = hashmap_init();
        if(lock_idp == NULL) {
            log_err("Cannot init hash map lock_idp");
            return NULL;
        }
    }
    lock_t *lock = (lock_t*)malloc(sizeof(lock_t));

    lock->id = get_next_lock_id();
    lock->waits = dlist_init();
    lock->owner = NULL;
    hashmap_put(lock_idp, lock->id, lock);
    return lock;
}

int locK_acquire(lock_t *lock, UserContext *user_context) {
    if(lock == NULL) {
        log_err("The lock pointer is NULL");
        return 1;
    }

    // Try acquire. If not acquired, wait
    if(lock->owner == NULL) {
        lock->owner = running_proc;
    } else {
        int rc = proc_enqueue(lock->waits, running_proc);
        if(rc) {
            log_err("Cannot add process PID(%d) to wait queue", running_proc->pid);
            return 1;
        }
        next_schedule(user_context);
    }

    if(lock->owner != running_proc) {
        log_err("Logic problem, PID(%d) is not the owner PID(%d) of lock %d", running_proc->pid, lock->owner->pid, lock->id);
    }

    log_info("PID(%d) acquire lock %d done!", running_proc->pid, lock->id);
    return 0;
}

int locK_release(lock_t *lock) {
    if(lock == NULL) {
        log_err("The lock pointer is NULL");
        return 1;
    }
   
    if(lock->owner != running_proc) {
        log_err("This lock doesn't belong to PID(%d)", running_proc->pid);
        return 1;
    }

    if(lock->waits->size) {
        lock->owner = dlist_rm_head(lock->waits);
        en_ready_queue(lock->owner);
    } else {
        lock->owner = NULL;
    }

    log_info("PID(%d) release lock %d done, now the lock has owner PID(%d)!", running_proc->pid, lock->id, lock->owner->pid);
    return 0;
}

int free_lock(lock_t *lock) {
    int id = lock->id;
    if(lock->owner != lock->id) {
        log_err("You cannot free some one else's lock");
        return 1;
    }
    if(lock->waits->size) {
        log_warn("The waits queue of lock %d is not empty", lock->id);
    }
    int rc = dlist_destroy(lock->waits);
    if(rc) {
        log_err("Cannot destroy lock waits queue");
        return 1;
    }
    free(lock);
    lock = NULL;
    id_generator_push(lock_id_list, id);
    return 0;
}

cvar_t *cvar_init() {
    if(cvar_idp == NULL) {
        cvar_idp = hashmap_init();
        if(cvar_idp == NULL) {
            log_err("Cannot init hash map lock_idp");
            return NULL;
        }
    }
    cvar_t *cvar = (cvar_t*)malloc(sizeof(cvar_t));

    cvar->id = get_next_cvar_id();
    cvar->waits = dlist_init();
    cvar->owner = running_proc;
    hashmap_put(cvar_idp, cvar->id, cvar);
    return cvar;
}

int cvar_wait(cvar_t* cvar, lock_t *lock, UserContext *user_context) {
    if(cvar == NULL) {
        log_err("cvar pointer is NULL");
        return 1;
    }
    if(lock == NULL) {
        log_err("lock pointer is NULL");
        return 1;
    }

    int rc = 0;
    log_info("PID(%d) about to release lock", running_proc->pid);
    locK_release(lock);
    log_info("PID(%d) release lock done", running_proc->pid);
    proc_enqueue(cvar->waits, running_proc);
    log_info("PID(%d) en waiting queue done", running_proc->pid);
    next_schedule(user_context);
    log_info("PID(%d) wake up from wait", running_proc->pid);
    lock_acquire(lock);
    log_info("PID(%d) lock acquire done", running_proc->pid);
    return 0;
}

int cvar_signal(cvar_t* cvar) {
    if(cvar == NULL) {
        log_err("cvar pointer is NULL");
        return 1;
    }

    if(cvar->waits->size) {
        pcb_t *next_proc = dlist_rm_head(cvar->waits);
        en_ready_queue(next_proc);
    }
    return 0; 
}

int cvar_broadcast(cvar_t* cvar) {
    if(cvar == NULL) {
        log_err("cvar pointer is NULL");
        return 1;
    }

    while(cvar->waits->size) {
        pcb_t *next_proc = dlist_rm_head(cvar->waits);
        en_ready_queue(next_proc);
    }
    return 0; 
}

int free_cvar(cvar_t *cvar) {
    int id = cvar->id;
    if(cvar->owner != cvar->id) {
        log_err("You cannot free some one else's cvar");
        return 1;
    }
    if(cvar->waits->size) {
        log_warn("The waits queue of cvar %d is not empty", cvar->id);
    }
    int rc = dlist_destroy(cvar->waits);
    if(rc) {
        log_err("Cannot destroy cvar waits queue");
        return 1;
    }
    free(cvar);
    cvar = NULL;
    id_generator_push(cvar_id_list, id);
    return 0;
}

int get_next_lock_id() {
    if(lock_id_list == NULL) {
        lock_id_list = id_generator_init(MAX_LOCKS);
    }
    return id_generator_pop(lock_id_list);
}

int get_next_cvar_id() {
    if(cvar_id_list == NULL) {
        cvar_id_list = id_generator_init(MAX_LOCKS);
    }

    return id_generator_pop(cvar_id_list);
}

