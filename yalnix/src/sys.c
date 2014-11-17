/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "sys.h"

//These are process syscalls
int Y_Fork(UserContext *user_context){
    pcb_t *child = init_user_proc(running_proc);
    if(child == NULL) {
        log_err("Init child process fail");
        return ERROR;
    }
    running_proc->user_context.regs[0] = child->pid;
    child->user_context.regs[0] = 0;
    
    int rc = copy_user_runtime(child, running_proc, user_context);
    if(rc) {
        free_proc(child);
        log_err("PID(%d) cannot copy PID(%d) runtime", child, running_proc);
        return ERROR;
    }
    
    en_ready_queue(running_proc);
    pick_schedule(user_context, child);
    running_proc->exit_code = running_proc->pid;
    
    if(running_proc == child) {
        return 0;
    } else {
        return child->pid;    
    }
}

int Y_Exec(char * filename, char* argvec[], UserContext *user_context){
    log_info("PID(%d) is going to exec %s.", running_proc->pid, filename);
    if(!ValidatePtr(filename, 1)) {
        log_err("filename pointer invalid!!");
        return ERROR;
    }
    log_info("Pass ValidatePtr!!");
    
    LoadProgram(filename, argvec, running_proc);
    *user_context = running_proc->user_context;
    return 0;
}

int Y_Exit(int exit_code, UserContext *user_context){
	pcb_t *parent = (pcb_t*)running_proc->parent;

    //Set states
    running_proc->exit_code = exit_code;
    running_proc->state = ZOMBIE;

    // Tell other people
    tell_children(running_proc);
    tell_parent(running_proc);

    // Scheulde next process to run
    next_schedule(user_context);
    log_info("Next schedule done (this will never reached actually)");
    return 0;
}

int Y_Wait(int * status_ptr, UserContext *user_context){
    if(ValidatePtr(status_ptr, 4) == 0) {
        log_err("Buff pointer invalid!!");
        return ERROR;
    }
    // Check if it is worth to wait
    if(!any_child_active(running_proc) && !running_proc->zombie->size) {
        log_info("Not woth to wait!! #zombie %d, and no children is active", running_proc->zombie->size);
        running_proc->exit_code = 1;
        return 1;       // No body treat pid 1 and child process, so I make it as exit_code
    }

    // Wait operations
    log_info("PID(%d) about to sleep until one of my child wake me up", running_proc->pid);
    en_wait_queue(running_proc);
    next_schedule(user_context);

    // Back to life!
    if(running_proc->wait_zombie) {
        log_info("PID(%d) wake up and going to run again!", running_proc->pid);
        pcb_t* zombie = de_zombie_queue(running_proc);
        log_info("PID(%d) get zombie %p!", running_proc->pid, zombie);
        *status_ptr = zombie->exit_code;
        //log_info("PID(%d) get zombie status!", running_proc->pid);
        running_proc->exit_code = zombie->pid; 
        //log_info("Get my zombie pid(%d) exit code %d", zombie->pid, zombie->exit_code);
        free_proc(zombie);
        running_proc->wait_zombie = 0;
        return running_proc->exit_code;
    } else {
        log_info("I am not waiting");
        return 0; 
    }

}

int Y_GetPid(UserContext *user_context){
    user_context->regs[0] = running_proc->pid;
    return user_context->regs[0];
}

int Y_Brk(uint32 addr){
    int page_cnt, rc; 
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bound = UP_TO_PAGE(new_addr);
    uint32 current_page_bound = UP_TO_PAGE(user_memory.brk_high);

    // Boudaries check
    if(new_addr > user_memory.stack_low - PAGESIZE) {
        log_err("New addr trepass the red zone below stack!");
        return _FAILURE;
    } // Check if trying to access below brk base line
    else if(new_addr < user_memory.brk_low) {
        log_err("New addr trepass the data area!");
        return _FAILURE;
    }   

    // Modify the brk 
    if(new_addr > user_memory.brk_high) { 
        page_cnt = GET_PAGE_NUMBER(new_page_bound) - GET_PAGE_NUMBER(current_page_bound);
        rc = map_page_to_frame(user_page_table, 
                        current_page_bound, 
                        page_cnt, 
                        PROT_READ | PROT_WRITE);
        if(rc) {
                log_err("User Break Warning: Not enough phycial memory\n");
                return _FAILURE;
        }   
        log_info("Y_Brk ADD DONE = %p, End = %p, New Addr = %p", user_memory.brk_low, user_memory.brk_high, addr);
    } else if (new_page_bound < user_memory.brk_high) {
        page_cnt = GET_PAGE_NUMBER(new_page_bound) - GET_PAGE_NUMBER(current_page_bound);
        rc = unmap_page_to_frame(user_page_table,
                            new_page_bound, 
                            page_cnt);
        if(rc) {
                log_err("User Break Warning: Not able to release pages\n");
                return _FAILURE;
        }   
    }   
    user_memory.brk_high = new_addr;
    log_info("PID %d user brk done, new addr at %p", running_proc->pid, new_addr);
    return _SUCCESS;
}

int Y_Delay(UserContext *user_context){
    int clock_ticks = user_context->regs[0];

	if(clock_ticks < 0){
		return _FAILURE;
	}
	
	if(clock_ticks == 0){
		return _SUCCESS;
	}

	running_proc -> ticks = clock_ticks;
    log_info("PID(%d) delay for %d sec", running_proc->pid, clock_ticks);

    en_delay_queue(running_proc);
    next_schedule(user_context);

    log_info("After delay, now is running PID(%d)", running_proc->pid);
	return _SUCCESS;
}

int Y_TtyWrite(int tty_id, void *buf, int len, UserContext *user_context)
{
    if(tty_id < 0 || tty_id > 3) {
        log_err("Invalid tty id: %d", tty_id);
        return ERROR;
    }
    if(ValidatePtr(buf, len) == 0) {
        log_err("Buff pointer invalid!!");
        return ERROR;
    }
    
    int commit_len;
    char *commit_buf;
    int result = 0;
    dnode_t *node = NULL;

    //log_info("tty_id = %d, buf = %p, len = %d, pid = %d", tty_id, buf, len, running_proc->pid);

    commit_buf = (void *)calloc(1, TERMINAL_MAX_LINE);
    if (commit_buf == NULL) {
            log_err("allocate buffer for tty transferring failed!");
            log_info("result = %d, pid = %d", result, running_proc->pid);
            return _FAILURE;
    }
    //log_info("calloc commit_buf done");

    len = min(len, strlen(buf));
    while (len) {
        commit_len = min(len, TERMINAL_MAX_LINE);
        memcpy(commit_buf, buf + result, commit_len);
        //log_info("memcpy from buff done");
            
        len -= commit_len;
        //log_info("TTY Enqueue PID(%d) DONE", running_proc->pid);
        while (is_write_busy(tty_id)) {
            tty_write_enqueue(running_proc, tty_id);
            pcb_t *print_proc = peek_tty_write_queue(tty_id);
            //log_info("PID(%d) is waiting for printing PID(%d)", running_proc->pid, tty_writing_procs[tty_id]->pid);
            next_schedule(user_context);
        }
        set_write_proc(running_proc, tty_id);
        //log_info("PID(%d) Right before tty transmit", running_proc->pid);
        TtyTransmit(tty_id, commit_buf, commit_len);
        //log_info("Right after tty transmit");
        next_schedule(user_context);
        //log_info("Back from tty block");
        result += commit_len;
    }

    free(commit_buf);

    return result;
}

int Y_TtyRead(int tty_id, void *buf, int len, UserContext *user_context)
{
    //log_info("Starts: tty_id = %d, buf = %p, len = %d, pid = %d", tty_id, buf, len, running_proc->pid);
    if(tty_id < 0 || tty_id > 3) {
        log_err("Invalid tty id: %d", tty_id);
        return ERROR;
    }
    if(ValidatePtr(buf, len) == 0) {
        log_err("Buff pointer invalid!!");
        return ERROR;
    }
    
    tty_read_enqueue(running_proc, tty_id);
    
    running_proc->tty_buf = (void *)calloc(1, len);
    if (running_proc->tty_buf == NULL) {
            log_err("Allocate buffer for tty reading failed!\n");
            log_info("Ends: result = %d, pid = %d", running_proc->exit_code, running_proc->pid);
            return _FAILURE;
    }
    running_proc->exit_code = len;
    next_schedule(user_context);

    memcpy(buf, running_proc->tty_buf, running_proc->exit_code);
    free(running_proc->tty_buf);
    running_proc->tty_buf = NULL;

    log_info("Number of chars get is %d, pid = %d", running_proc->exit_code, running_proc->pid);

    return running_proc->exit_code;
}

int Y_PipeInit(int *pipe_idp) {
    if(pipe_init(pipe_idp)) {
        return ERROR;
    }
    return 0;
}

int Y_PipeRead(int pipe_id, void *buf, int len, UserContext *user_context) {
    if(ValidatePtr(buf, len) == 0) {
        log_err("Buff pointer invalid!!");
        return ERROR;
    }
    log_info("PID(%d) going to read from pipe %d", running_proc->pid, pipe_id);

    pipe_t *pipe = (pipe_t*)util_get(pipe_id);
    log_info("Got pipe pointer %p", pipe);
    if(pipe == NULL) {
        log_err("Cannot get pipe %d from hashmap", pipe->id);
    }

    return pipe_read(pipe, buf, len, user_context);
}

int Y_PipeWrite(int pipe_id, void *buf, int len, UserContext *user_context) {
    if(ValidatePtr(buf, len) == 0) {
        log_err("Buff pointer invalid!!");
        return ERROR;
    }
    log_info("PID(%d) going to write to pipe %d", running_proc->pid, pipe_id);
    
    pipe_t *pipe = (pipe_t*)util_get(pipe_id);
    if(pipe == NULL) {
        log_err("Cannot get pipe %d from hashmap", pipe->id);
    }
    
    return pipe_write(pipe, buf, len, user_context);
}

int Y_LockInit(int *lock_idp) {
    if(lock_init(lock_idp)) {
        return ERROR;
    }
    return 0;
}

int Y_Acquire(int id, UserContext *user_context) {
    lock_t *lock = (lock_t*)util_get(id);
    if(lock == NULL) {
        log_err("Error getting lock id %d", id);
        return -1;
    }
    return lock_acquire(lock, user_context);
}

int Y_Release(int id) {
    lock_t *lock = (lock_t*)util_get(id);
    if(lock == NULL) {
        log_err("Error getting lock id %d", id);
        return -1;
    }
    return lock_release(lock);
}

int Y_Reclaim(int id) {
    util_t *util = util_rm(id);
    if(util == NULL) {
        log_err("Unable to remove util, id %d", id);
        return ERROR;
    }

    int rc = 0;
    void * data = util->data;
    switch(util->type) {
        case LOCK:
            rc = free_lock((lock_t*)data);
            break;
        case CVAR:
            rc = free_cvar((cvar_t*)data);
            break;
        case PIPE:
            rc = free_pipe((pipe_t*)data);
            break;
        default:
            break;
    }

    if(rc) {
        log_err("Fail to release util %d", id);
        return ERROR;
    }
    return 0;
}

int Y_CvarInit(int *cvar_idp) {
    if(cvar_init(cvar_idp)){
        return ERROR;
    }
    return 0;
}

int Y_CvarSignal(int id) {
    cvar_t *cvar = (cvar_t*)util_get(id);
    if(cvar == NULL) {
        log_err("Error getting cvar id %d", id);
        return -1;
    }
    return cvar_signal(cvar);
}

int Y_CvarBroadcast(int id) {
    cvar_t *cvar = (cvar_t*)util_get(id);
    if(cvar == NULL) {
        log_err("Error getting cvar id %d", id);
        return -1;
    }
    return cvar_broadcast(cvar);
}

int Y_CvarWait(int cid, int lid, UserContext *user_context) {
    cvar_t *cvar = (cvar_t*)util_get(cid);
    if(cvar == NULL) {
        log_err("Error getting cvar id %d", cid);
        return -1;
    }
    
    lock_t *lock = (lock_t*)util_get(lid);
    log_info("Get lock at %p", lock);
    if(lock == NULL) {
        log_err("Error getting lock id %d", lid);
        return -1;
    }
 
    return cvar_wait(cvar, lock, user_context);
}

int Y_GetPipeSize(int pipe_id) {
    log_info("PID(%d) going to write to pipe %d", running_proc->pid, pipe_id);
    
    pipe_t *pipe = (pipe_t*)util_get(pipe_id);
    if(pipe == NULL) {
        log_err("Cannot get pipe %d from hashmap", pipe->id);
    }

    return get_buff_size(pipe);
}

int ValidatePtr(void *ptr, int length){
    //pointer should not be NULL and length should at least 0
    log_info("Inside %s", __func__);
    log_info("PTR is %p, length is %d", ptr, length);
    if(ptr == NULL || length <= 0){
        log_err("Error: NULL PTR or non positive Length");
        return 0;
    }
    //Make sure the pointer is in user space
    if((int)ptr < VMEM_1_BASE){
        log_err("Error: PTR in Kernel Space\n");
        return 0;
    }else if((int)ptr >= VMEM_1_LIMIT){
        log_err("Error: PTR over User Space\n");
        return 0;
    }
    //Check the start page and end page of the buffer
    //int startPage = (int)(ptr - VMEM_1_BASE) >> PAGESHIFT;
    //int endPage = (int)(ptr - VMEM_1_BASE + length - 1) >> PAGESHIFT;
    //log_info("Validating Ptr %p with Length %d\n", ptr, length);
    return 1;
}

int ValidateCStyle(void *ptr, int type){
    //Validate each element
    while(1){
	//the buffer is at right place of memory
	int result = ValidatePtr(ptr, type);
	if(result == 0)
	    return ERROR;
    	//if the element inside is char then the original will be a char[] and will end with '\0'
	if(type == sizeof(char)){
	    if(*((char *)ptr) == '\0')
	        return 0;
            ptr = ((char *)ptr) + 1;
	//if the element inside is char pointer Then the origianl one will be a pointer to an array og pointer(char**) End with NULL
	}else if(type == sizeof(char *)){
	    if(*((char **)ptr) == NULL)
		return 0;
	    ptr = ((char **)ptr) + 1;
	}
    }
}
////These are 3 custom syscalls
//int Y_Custom0(){
//	
//}
//
//int Y_Custom1(){
//	
//}
//
//int Y_Custom2(){
//	
//}
//
//int Y_Nop(int, int, int, int){
//
//}
//
//int Y_Boot(){
//
//}
//
//int Y_Register(unsigned int **){
//	
//}
//
//int Y_Send(void* , int){
//	
//}
//
//int Y_Receive(void *){
//
//}
//
//int Y_ReceiveSpecific(void *, int ){
//
//}
//
//int Y_Reply (void *, int){
//
//}
//
//int Y_Forward (void *, int, int){
//
//}
//
//int Y_CopyFrom (int, void *, void *, int){
//
//}
//
//int Y_CopyTo (int, void *, void *, int){
//
//}
//
//int Y_ReadSector (int, void *){
//
//}
//
//int Y_WriteSector (int, void *){
//
//}
