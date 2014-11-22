/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 2

void main(int argc, char **argv) {
    user_log("test.pipe program has PID(%d)", GetPid());
    int pipe_id; 
    PipeInit(&pipe_id);
    char buf[100];

    TtyPrintf(TTY_ID, "Pipe init done, with pipe id: %d\n", pipe_id);
    int pid = Fork();
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        TtyPrintf(TTY_ID, "Enter 2 chars (create some offset), PID(%d) can write to pipe id %d:\n", GetPid(), pipe_id);
        TtyRead(TTY_ID, buf, 2);
        int writecnt = PipeWrite(pipe_id, buf, 2);
        TtyPrintf(TTY_ID, "PID(%d) pipe write %d done\n", GetPid(), writecnt);
       
        //Delay(10);
        TtyPrintf(TTY_ID, "Enter something so that PID(%d) can write to pipe id %d:\n", GetPid(), pipe_id);
        TtyRead(TTY_ID, buf, 12);
        writecnt = PipeWrite(pipe_id, buf, 12);
        TtyPrintf(TTY_ID, "PID(%d) pipe write %d done\n", GetPid(), writecnt);
        TtyPrintf(TTY_ID, "PID(%d) is going to wait for children\n", GetPid());
        Wait();
        TtyPrintf(TTY_ID, "PID(%d) is going to reclaim pipe %d \n", GetPid(), pipe_id);
        int rc = Reclaim(pipe_id);
        if(rc) {
            TtyPrintf(TTY_ID, "PID(%d) reclaim pipe %d fail\n", GetPid(), pipe_id);
        }
        TtyPrintf(TTY_ID, "PID(%d) reclaim pipe %d done\n", GetPid(), pipe_id);
        TtyPrintf(TTY_ID, "End of pipe test");
    } else {
        int readcnt = PipeRead(pipe_id, buf, 2);
        //Delay(2);   // Delay to avoid overlap printing
        TtyPrintf(TTY_ID, "PID(%d) get contents from pipe id %d: %s, %d read\n", GetPid(), pipe_id, buf, readcnt);
        
        //Delay(10);
        readcnt = PipeRead(pipe_id, buf, 12);
        //Delay(2);   // Delay to avoid overlap printing
        TtyPrintf(TTY_ID, "PID(%d) get contents from pipe id %d: %s, %d read\n", GetPid(), pipe_id, buf, readcnt);
    }

    Exit(0);

    // Never reached
    return;
}



