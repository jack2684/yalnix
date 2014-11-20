/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 2

void main(int argc, char **argv) 
{
	int pid, sem_id, i, rc;
	int value = 0;
    
    user_log("test.sem program has PID(%d)", GetPid());
    rc = SemInit(&sem_id, value);

    TtyPrintf(TTY_ID, "%d: Successfully created sem with sem id %d\n", GetPid(), sem_id);
    i = 3;
    pid = Fork();
    if (pid)
    {
      pid = Fork();
    }

    while (--i) 
    {
		TtyPrintf(TTY_ID, "%d: Attempting sem %d.\n", GetPid(), sem_id);
		rc = SemDown(sem_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Sem down Failed\n");
			Exit(1);
    	}

		TtyPrintf(TTY_ID, "%d: Sem Down %d.\n", GetPid(), sem_id);	
		Delay(2);
		rc = SemUp(sem_id);
		if (rc == _FAILURE) 
		{
			TtyPrintf(TTY_ID, "Sem Up failed\n");
			Exit(1);
	    }
	    TtyPrintf(TTY_ID, "%d: Sem Up %d\n", GetPid(), sem_id);

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
    Wait(&status); 
    Wait(&status); 
    TtyPrintf(TTY_ID, "%d: Waiting done\n", GetPid());

	rc = Reclaim(sem_id);
	if (rc == _FAILURE) 
	{
		TtyPrintf(TTY_ID, "Reclaim sem failed.\n");
		Exit(1);
	}
    
    TtyPrintf(TTY_ID, "%d: Successfully reclaimed sem.\n", GetPid());
    TtyPrintf(TTY_ID, "%d: End sem test.\n", GetPid());
  	Exit(0);

  	return;
}
