#include "kernelLib.h"

//These are Pipe syscalls
int Y_PipeInit(int *pipe_idp){
	//CREATE a new pipe
	//ADD the new pipe
	//SET pipe_id pointer point to the pipe
	//RETURN success
}

int Y_PipeWrite(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id
	
    // While(pipe->len <= 0) {}
    
    // Read data from pipe->buff

    // pipe->readIdx = (pipe->readIdx + len ) % pipe->max_len

    //COPY data into pipe
	//RETURN len
}

int Y_PipeWrite(int pipe_id, void *buf, int len){
	//CHECK len & buf are valid
	//FIND the pipe using id

    //WHILE ((pipe->len + len) >= pipe->max_len) {}

    // Write data
    
    // pipe->len += len;
    
    // pipe->wirteIdx = (pipe->writeIdx + len ) % pipe->max_len


	//RETURN the number of readed bytes
}
