#include "kernelLib.h"

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
