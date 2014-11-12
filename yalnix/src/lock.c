/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "lock.h"

hashmap_t *lock_idp = NULL;
dlist_t *lock_id_list = NULL;

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
        int rc = dlist_add_tail(lock->waits, running_proc);
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

int locK_release(lock_t *lock, UserContext *user_context) {
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

int get_next_lock_id() {
    if(lock_id_list == NULL) {
        lock_id_list = id_generator_init(MAX_LOCKS);
    }

    return id_generator_pop(lock_id_list);
}

