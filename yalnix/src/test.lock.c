/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 2
//test int LockInit(int *lock_idp)
//test int Acquire(int lock id)
//test int Release(int lock id)
//test int Reclaim(int id)

void main(int argc, char **argv) 
{
	int pid, lock_id, i, rc;
    
    user_log("test.lock program has PID(%d)", GetPid());
    int rc = LockInit(&lock_id);

    TtyPrintf(TTY_ID, "%d: Successfully created lock with lock id %d\n", GetPid(), lock_id);
    i = 3;
    pid = Fork();
    if (pid)
    {
      pid = Fork();
    }

    while (--i) 
    {
		//TtyPrintf(TTY_ID, "%d: Attempting to acquire lock id %d\n", GetPid(), lock_id);
		rc = Acquire(lock_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Acquire lock Failed\n");
			Exit(1);
    	}

		TtyPrintf(TTY_ID, "%d: Acquired. Delay for 2 ticks and release the lock.\n", GetPid());
		Delay(2);
		//TtyPrintf(TTY_ID, "%d: Back from delay.\n", GetPid());
		
		rc = Release(lock_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Release lock failed\n");
			Exit(1);
	    }

	    //TtyPrintf(TTY_ID, "%d: Return value for previous attempt was %d\n", GetPid(), rc);
	    TtyPrintf(TTY_ID, "%d: Lock successfully released and delay for 2 ticks\n", GetPid());
	    Delay(2);
	} 
  
    if (!pid)
    {
     	Exit(0);
    }

    Delay(6);


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