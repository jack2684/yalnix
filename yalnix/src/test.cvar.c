/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 2
//test int CvarInit(int *cvar idp)
//test int CvarWait(int cvar id, int lock id)
//test int Reclaim(int id)

void main(int argc, char **argv) 
{
	int pid,cvarid,lockid;
	user_log("test_temp main:\n");

	LockInit(&lockid);	
	CvarInit(&cvarid);
	
	pid = Fork();
	if(pid==0){
		TtyPrintf(TTY_ID, "child: acquire lock\n");
		Acquire(lockid);
		TtyPrintf(TTY_ID, "child: reclaim cvar\n");
		Reclaim(cvarid);
		TtyPrintf(TTY_ID, "child: exit\n");
		Exit(0);
	}
	else{
		TtyPrintf(TTY_ID, "parent: acquire\n");
		Acquire(lockid);
		TtyPrintf(TTY_ID, "parent: wait\n");
		CvarWait(cvarid,lockid);
		Release(lockid);
	}
	Exit(0);
	return;
}