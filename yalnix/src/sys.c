/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "sys.h"

//These are process syscalls
int Y_Fork(UserContext *user_context){
    pcb_t *child = init_user_proc(running_proc);
    running_proc->user_context.regs[0] = child->pid;
    child->user_context.regs[0] = 0;
    
    int rc = copy_user_runtime(child, running_proc, user_context);
    if(rc) {
        log_err("PID(%d) cannot copy PID(%d) runtime", child, running_proc);
        return -1;
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
    log_info("PID(%d) is going to exec %s", running_proc->pid, filename);
    LoadProgram(filename, argvec, running_proc);
    *user_context = running_proc->user_context;
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
}

int Y_Wait(int * status_ptr, UserContext *user_context){
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
        *status_ptr = zombie->exit_code;
        running_proc->exit_code = zombie->pid; 
        log_info("Get my zombie pid(%d) exit code %d", zombie->pid, zombie->exit_code);
        free_proc(zombie);
        running_proc->wait_zombie = 0;
        return running_proc->exit_code;
    } else {
        log_info("I am not waiting");
        return 0; 
    }


	//IF there are zombie children
		//REMOVE their PCB from PCB list
		//FREE PCB
	//END IF

	//IF there are no live children
		//REPORT ERROR
	//END IF

	//BLOCK current process
}

int Y_WaitPid(int pid, int* status_ptr, int options){
	//IF pid < -1
		//WAIT for any child process in a process group whose pid is -pid
	//END IF	
	
	//IF pid == -1
		//CALL Wait()
	//END IF
	
	//IF pid == 0
		//WAIT for any child process in the same process group as the parent process
	//END IF

	//IF pid > 0
		//WAIT for the child process whose pid equals the given pid argument
	//END IF
	
	//SET the options argument to the default number 0
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
        int commit_len;
        char *commit_buf;
        int result = 0;
        trans_finish = 0;
        dnode_t *node = NULL;

        log_info("tty_id = %d, buf = %p, len = %d, pid = %d", tty_id, buf, len, running_proc->pid);

        while (!dlist_is_empty(tty_trans_queues[tty_id])) {
                running_proc -> state = WAIT;
                tty_trans_enqueue(running_proc, tty_id);
                //while ends conditions: running_proc is the head
                node = tty_trans_queues[tty_id] -> head;
                if ((pcb_t *)(node -> data) == running_proc)
                {
                    break;
                }
                next_schedule(user_context);
        }

        //check memory allocation
        commit_buf = (void *)calloc(1, TERMINAL_MAX_LINE);
        if (commit_buf == NULL) {
                log_err("allocate buffer for tty transferring failed!\n");
                log_info("result = %d, pid = %d", result, running_proc->pid);
                return _FAILURE;
        }

        //if buf exceeds the limit, then do tty-writing multiple times //
        len = min(len, strlen(buf));
        while (len) {
                commit_len = min(len, TERMINAL_MAX_LINE);
                memcpy(commit_buf, buf + result, commit_len);
                
                //running_proc -> state = WAIT;
                //tty_writing_procs[tty_id] = running_proc;
                len -= commit_len;
                if(len <= 0)
                {
                    trans_finish = 1;
                }
                TtyTransmit(tty_id, commit_buf, commit_len);
                next_schedule(user_context);
                result += commit_len;
        }

        free(commit_buf);

        log_info("result = %d, pid = %d\n", result, running_proc->pid);
        return result;
}

int Y_TtyRead(int tty_id, void *buf, int len, UserContext *user_context)
{
        log_info("Starts: tty_id = %d, buf = %p, len = %d, pid = %d", tty_id, buf, len, running_proc->pid);

        len = min(len, TERMINAL_MAX_LINE);

        dnode_t *node = NULL;

        //there is only one process reading from tty_id //
        while (!dlist_is_empty(tty_read_queues[tty_id])) 
        {
                running_proc -> state = WAIT;
                tty_read_enqueue(running_proc, tty_id);
                node = tty_read_queues[tty_id] -> head;
                if ((pcb_t *)(node -> data) == running_proc)
                {
                    break;
                }
                next_schedule(user_context);
        }

        running_proc->tty_buf = (void *)calloc(1, len);
        
        //check memory allocation
        if (running_proc->tty_buf == NULL) {
                log_err("Allocate buffer for tty reading failed!\n");
                log_info("Ends: result = %d, pid = %d", running_proc->exit_code, running_proc->pid);
                return _FAILURE;
        }

        //tty_reading_procs[tty_id] = running_proc;
        running_proc->exit_code = len;
        running_proc -> state = WAIT;
        next_schedule(user_context);

        memcpy(buf, running_proc->tty_buf, running_proc->exit_code);
        free(running_proc->tty_buf);
        running_proc->tty_buf = NULL;

        log_info("Ends: result = %d, pid = %d", running_proc->exit_code, running_proc->pid);
        return running_proc->exit_code;
}

////These are destroy syscalls
//int Y_Reclaim(int id){
//	//FIND the lock using id
//	//DESTROY the lock
//
//	//FIND the cvar using id
//	//DESTROY the cvar
//
//	//FIND the pipe using id
//	//DESTROY the pipe
//}
//
////Here are some extra syscalls below that could help implement some extra functions
////We will complete them if possible
//
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
