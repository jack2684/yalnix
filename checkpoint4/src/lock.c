/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "kernelLib.h"

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

//These are semaphore syscalls
int Y_SemInit(int *, int){

}

int Y_SemUp (int){

}

int Y_SemDown (int){

}
