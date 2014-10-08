//All these syscalls are named with the prefix "Yalnix_"

//These are process syscalls
int Fork(void){
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

int Exec(char * filename, char* argvec[]){
	//CHECK if filename is valid
	//COPY filename into the heap
	//CHECK if arguments are valid
	//COPY arguments into the heap
	//LOAD text into new page table
	//COPY the user context
	//FREE the content in heap
}

int Exit(int status){
	//WHILE lock list is not empty
		//RELEASE all the locks
	//END WHILE

	//WHILE child process PCB list is not empty
		//SET parent pointers to the children to NULL
		//RELEASE all the children's PCB
	//END WHILE	

	//RELEASE all the buffers
	//RELEASE all the frames

	//IF there is still a parent
		//MOVE process to its zombie children list
	//ELSE
		//RELEASE PCBs
	//END IF

	//SWITCH to user-context
}

int Wait(int * status_ptr){
	//IF there are zombie children
		//REMOVE their PCB from PCB list
		//FREE PCB
	//END IF

	//IF there are no live children
		//REPORT ERROR
	//END IF

	//BLOCK current process
	//RUN next process
}

int GetPid(void){
	//RETURN current process's pid
}

int Brk(void * addr){
	//IF new break page is larger than current user stack page size
		//REPORT ERROR
	//END IF

	//IF new break page is larger than current user break page
		//ENLARGE the user heap
	//END IF
}

int Delay(int clock_ticks){
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
int TtyRead(int tty_id, void *buf, int len){
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

int TtyWrite(int tty_id, void *buf, int len){
	//CHECK len is valid
	//CHECK buf is valid
	//SET tty transmit length to len
	//COPY len bytes of buffer into the heap
	//WRITE to terminal
	//RETURN len
}

//These are Pipe syscalls
int PipeInit(int *pipe_idp){
	//CREATE a new pipe
	//PUT pipe's id into the pointer
	//RETURN success
}

int PipeRead(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id
	
	//WHILE pipe's available chars is less than len
		//BLOCK the calling process
	//END WHILE

	//RETURN the number of readed bytes
}

int PipeWrite(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id

	//IF len is larger than buffer capacity
		//INCREASE buffer size
	//END IF

	//COPY data into pipe
	//RETURN len
}

//These are lock syscalls
int LockInit(int *lock_idp){
	//CREATE a new lock
	//Save the lock id into the poiner
}

int Acquire(int lock_id){
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

int Release(int lock_id){
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
int CvarInit(int *cvar_idp){
	//CREATE a new cvar
	//COPY the cvar's id into the pointer
	//RETURN
}

int CvarSignal(int cvar_id){
	//FIND the cvar with id

	//IF no process waiting on the cvar
		//RETURN
	//END IF

	//REMOVE a process from the waiting queue 
	//SET the process ready
}

int CvarWait(int cvar_id, int lock_id){
	//FIND the cvar with id

	//RELEASE the lock

	//ADD the current process to the cvar's waiting list

	//REACQUIRE the lock
}

int CvarBroadcast(int cvar_id){
	//FIND the cvar with id

	//FOR all process in the cvar's waiting queue
		//REMOVE from the queue
		//SET to ready
	//END FOR  
}

//These are destroy syscalls
int Reclaim(int id){
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
int Custom0(){
	
}

int Custom1(){
	
}

int Custom2(){
	
}

//These are semaphore syscalls
int SemInit(int *, int){

}

int SemUp (int){

}

int SemDown (int){

}

int Nop(int, int, int, int){

}

int Boot(){

}

int Register(unsigned int **){
	
}

int Send(void* , int){
	
}

int Receive(void *){

}

int ReceiveSpecific(void *, int ){

}

int Reply (void *, int){

}

int Forward (void *, int, int){

}

int CopyFrom (int, void *, void *, int){

}

int CopyTo (int, void *, void *, int){

}

int ReadSector (int, void *){

}

int WriteSector (int, void *){

}


int TtyPrintf(int, char *, ...){

}

