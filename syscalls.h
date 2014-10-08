//All these syscalls are named with the prefix "Y_"

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

int Y_Exit(int status){
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
	//RETURN current process's pid
}

int Y_Brk(void * addr){
	//IF new break space impose on the  current user stack space
		//REPORT ERROR
	//END IF

	//IF new break segment is larger than current user break segment
		//ENLARGE the user heap
	//END IF

	//IF new break segment is smaller than the current user break segment
		//SHRINK the heap space
	//END IF
}
 
int Y_Delay(int clock_ticks){
	//IF clock_ticks < 0
		//REPORT ERROR
	//END IF

	//IF clock_ticks == 0
		//RETURN 
	//END IF

	//SET remaining clock_ticks to the current clock_ticks
	//BLOCK the calling process with the clock_ticks
}

//These are tty syscalls
int Y_TtyRead(int tty_id, void *buf, int len){
	//CHECK len is valid
	//CHECK buffer is valid

	//IF tty has more than len bytes to input
		//CONSUME only the length of len bytes
		//ADD remaining bytes into the waiting queue
	//ELSE
		//CONSUME all the bytes input
	//END IF

	//RETURN received bytes' length 
}

int Y_TtyWrite(int tty_id, void *buf, int len){
	//CHECK len is valid
	//CHECK buf is valid
	//SET tty transmit length to len
	//COPY len bytes of buffer into the heap
	//WRITE to terminal
	//RETURN len
}

//These are Pipe syscalls
int Y_PipeInit(int *pipe_idp){
	//CREATE a new pipe
	//ADD the new pipe
	//SET pipe_id pointer point to the pipe
	//RETURN success
}

int Y_PipeRead(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id
	
	//WHILE pipe's available chars is less than len
		//BLOCK the calling process
	//END WHILE

	//RETURN the number of readed bytes
}

int Y_PipeWrite(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id

	//IF len is larger than buffer capacity
		//INCREASE buffer size
	//END IF

	//COPY data into pipe
	//RETURN len
}

//These are lock syscalls
int Y_LockInit(int *lock_idp){
	//CREATE a new lock
	//Save the lock id into the poiner
}

int Y_Acquire(int lock_id){
	//FIND the lock using id

	//IF already have the lock
		//RETURN
	//END IF

	//IF the lock is available
		//OBTAIN the lock
		//RETURN
	//ELSE add to the waiting queue
	//END IF
}

int Y_Release(int lock_id){
	//FIND the lock using id

	//IF already have the lock
		//SET the lock availbale
		//RETURN
	//END IF

	//POP a process from the waiting queue
	//GIVE this process the lock
	//SET the process ready
}

//These are condition variables syscalls
int Y_CvarInit(int *cvar_idp){
	//CREATE a new cvar
	//COPY the cvar's id into the pointer
	//RETURN
}

int Y_CvarSignal(int cvar_id){
	//FIND the cvar with id

	//IF no process waiting on the cvar
		//RETURN
	//END IF

	//REMOVE a process from the waiting queue 
	//SET the process ready
}

int Y_CvarWait(int cvar_id, int lock_id){
	//FIND the cvar with id

	//RELEASE the lock

	//ADD the current process to the cvar's waiting list

	//REACQUIRE the lock
}

int Y_CvarBroadcast(int cvar_id){
	//FIND the cvar with id

	//FOR all process in the cvar's waiting queue
		//REMOVE from the queue
		//SET to ready
	//END FOR  
}

//These are destroy syscalls
int Y_Reclaim(int id){
	//FIND the lock using id
	//DESTROY the lock

	//FIND the cvar using id
	//DESTROY the cvar

	//FIND the pipe using id
	//DESTROY the pipe
}

//Below is some extra syscalls that could help implements some extra functions
//We will complete them if possible

//These are 3 custom syscalls
int Y_Custom0(){
	
}

int Y_Custom1(){
	
}

int Y_Custom2(){
	
}

//These are semaphore syscalls
int Y_SemInit(int *, int){

}

int Y_SemUp (int){

}

int Y_SemDown (int){

}

int Y_Nop(int, int, int, int){

}

int Y_Boot(){

}

int Y_Register(unsigned int **){
	
}

int Y_Send(void* , int){
	
}

int Y_Receive(void *){

}

int Y_ReceiveSpecific(void *, int ){

}

int Y_Reply (void *, int){

}

int Y_Forward (void *, int, int){

}

int Y_CopyFrom (int, void *, void *, int){

}

int Y_CopyTo (int, void *, void *, int){

}

int Y_ReadSector (int, void *){

}

int Y_WriteSector (int, void *){

}


int Y_TtyPrintf(int, char *, ...){

}

