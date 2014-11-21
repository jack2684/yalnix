/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 3
//test int LockInit(int *lock_idp)
//test int Acquire(int lock id)
//test int Release(int lock id)
//test int Reclaim(int id)

void main(int argc, char **argv) 
{
	int pid, lock_id, i, rc;
    
    user_log("test.lock program has PID(%d)", GetPid());
    LockInit(&lock_id);

    TtyPrintf(TTY_ID, "%d: Successfully created lock with lock id %d\n", GetPid(), lock_id);
    i = 3;
    pid = Fork();
    if (pid)
    {
      pid = Fork();
    }

    while (--i) 
    {
		TtyPrintf(TTY_ID, "%d: Attempting lock %d.\n", GetPid(), lock_id);
		rc = Acquire(lock_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Acquire lock Failed\n");
			Exit(1);
    	}

		TtyPrintf(TTY_ID, "%d: Acquired lock %d.\n", GetPid(), lock_id);
		Delay(2);
		//TtyPrintf(TTY_ID, "%d: Back from delay.\n", GetPid());
		
		rc = Release(lock_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Release lock failed\n");
			Exit(1);
	    }
	    TtyPrintf(TTY_ID, "%d: Released lock %d\n", GetPid(), lock_id);

	    //TtyPrintf(TTY_ID, "%d: Return value for previous attempt was %d\n", GetPid(), rc);
	    Delay(2);
	} 
  
    TtyPrintf(TTY_ID, "%d: while done\n", GetPid());
    if (!pid)
    {
     	Exit(0);
    }

    int status;
    TtyPrintf(TTY_ID, "%d: Waiting children\n", GetPid());
    Wait(&status); // Wait for children to be done
    Wait(&status); // Wait for children to be done
    TtyPrintf(TTY_ID, "%d: Waiting done\n", GetPid());


	//TtyPrintf(TTY_ID, "%d: Attempting to reclaim lock.\n", GetPid());
	rc = Reclaim(lock_id);
	if (rc == _FAILURE) 
	{
		TtyPrintf(TTY_ID, "Reclaim lock failed.\n");
		Exit(1);
	}
    
    TtyPrintf(TTY_ID, "%d: Successfully reclaimed lock.\n", GetPid());
    TtyPrintf(TTY_ID, "%d: End lock test.\n", GetPid());
  	Exit(0);

  	return;
}
