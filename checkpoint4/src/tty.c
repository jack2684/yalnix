/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "kernelLib.h"

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
