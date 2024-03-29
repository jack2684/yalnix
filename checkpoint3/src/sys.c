#include "sys.h"
#include "common.h"
#include "proc.h"

//These are process syscalls
int Y_Fork(void){
	//SAVE the current user-context
	//COPY Parent's user-context
	//COPY address space
	//GET child's pid

	//IF child's pid equals current pid
		//RETURN 0
	//ELSE
		//RETURN 1
	//END IF
}

int Y_Exec(char * filename, char* argvec[]){
	//CHECK if filename is valid
	//COPY filename into the heap
	//CHECK if arguments are valid
	//COPY arguments into the heap
	//INIT all the text, data, stack segments
	//RENEW the user context
	//FREE the content in heap
}

int Y_Exit(UserContext *user_context){
	//WHILE lock list is not empty
		//RELEASE all the locks
	//END WHILE

	//WHILE child process PCB list is not empty
		//SET the id  of parent process of the children to 1 (INIT)
		//IF a child process is zombie
			//CLEAN this child process
		//END IF
	//END WHILE	

	//RELEASE all the buffers
	//RELEASE all the frames

	//IF there is still a parent
		//MOVE process to its zombie children list
	//ELSE
		//RELEASE PCBs
	//END IF
}

int Y_Wait(int * status_ptr){
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

int Y_GetPid(void){
    return running_proc -> pid;
}

int Y_Brk(void * addr){
    log_info("PID %d call user brk with current brk %p", running_proc->pid, user_memory.brk_high);
    int page_cnt, rc; 
    uint32 new_addr = (uint32)addr;
    uint32 new_page_bound = UP_TO_PAGE(new_addr);
    uint32 current_page_bound = UP_TO_PAGE(user_memory.brk_high);

    // Boudaries check
    if(new_addr > user_memory.stack_low - PAGESIZE) {
        log_err("New addr trepass the stack area!");
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
        TracePrintf(0, "Y_Brk ADD DONE = %p, End = %p, New Addr = %p\n", user_memory.brk_low, user_memory.brk_high, addr);
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
    log_info("PID %d delay for %d sec", running_proc->pid, clock_ticks);

    en_delay_queue(running_proc);
    next_schedule(user_context);
    pcb_t* tmp = delay_queue->head->data;

	return _SUCCESS;
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
