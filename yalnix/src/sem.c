#include "sem.h"

sem_t *sem_init(int value)
{
	sem_t * sem = (sem_t *)malloc(sizeof(sem_t));
	
	if(sem == NULL)
	{
		log_err("Init sem failed.\n");
		return NULL;
	}

	sem -> id = util_new_id();
	if(sem -> id < 0)
	{
		log_err("Cannot get sem id.\n");
		return NULL;
	}
	log_info("Get sem id %d\n", sem -> id);

	sem -> waits = dlist_init();
	sem -> owner = NULL;
	sem -> value = value;
	util_add(sem -> id, sem, SEM);

	return sem; 
}

int sem_down(sem_t *sem, UserContext *user_context)
{
	if(sem == NULL)
	{
		log_err("The sem pointer is NULL");
        return 1;
	}
	
	if(sem -> owner == NULL) 
	{
	        sem -> owner = running_proc;
    	}
	else
	{
		sem -> value --;

		if(sem -> value < 0)
		{
			int rc = proc_enqueue(sem -> waits, running_proc);
			if(rc)
			{
				log_err("Cannot add process PID(%d) to wait queue", running_proc->pid);
		    return 1;
			}
			next_schedule(user_context);
		}
	}
	
	if(sem -> owner != running_proc)
	{
		log_err("Logic problem, PID(%d) is not the owner PID(%d) of sem %d", running_proc -> pid, sem -> owner -> pid, sem -> id);
	}

	log_info("PID(%d) wait sem %d done!", running_proc -> pid, sem -> id);
    return 0;
}

int sem_up(sem_t *sem)
{
	if(sem == NULL)
	{
		log_err("The sem pointer is NULL");
        return 1;
	}
	
	if(sem -> owner != running_proc)
	{	
		log_err("This sem doesn't belong to PID(%d)", running_proc->pid);
        	return 1;
	}
	sem -> value ++;
	
	if(sem -> waits -> size)
	{
		if(sem -> value <= 0)
		{
			sem -> owner = proc_dequeue(sem -> waits);
			en_ready_queue(sem -> owner);
			log_info("PID(%d) signal sem %d done, now the sem has owner PID(%d)!", running_proc -> pid, sem -> id, sem -> owner -> pid);
		}
	}else
	{
		sem -> owner = NULL;
        	log_info("PID(%d) release sem %d done, now the sem is empty!", running_proc -> pid, sem -> id);
	}
	return 0;
}

int free_sem(sem_t *sem)
{
	int id = sem -> id;
    if(sem -> owner != running_proc) {
        log_err("You cannot free some one else's sem");
        return 1;
    }

    if(sem -> waits -> size) {
        log_warn("The waits queue of sem %d is not empty", sem -> id);
    }
    int rc = dlist_destroy(sem -> waits);
    if(rc) {
        log_err("Cannot destroy sem waits queue");
        return 1;
    }
    free(sem);
    sem = NULL;
    util_reclaim_id(id);
    return 0;
}
