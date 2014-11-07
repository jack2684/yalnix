/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 2

void main(int argc, char **argv) {
    user_log("test.pipe program has PID(%d)", GetPid());
    int pipe_id = PipeInit();
    char buf[100];

    TtyPrintf(TTY_ID, "Pipe init done, with pipe id: %d\n", pipe_id);
    int pid = Fork();
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        TtyPrintf(TTY_ID, "Enter something so that PID(%d) can write to pipe id %d:\n", GetPid(), pipe_id);
        TtyRead(TTY_ID, buf, 10);
        PipeWrite(pipe_id, buf, 10);
        TtyPrintf(TTY_ID, "PID(%d) pipe write done\n", GetPid());
    } else {
        PipeRead(pipe_id, buf, 10);
        Delay(2);   // Delay to avoid overlap printing
        TtyPrintf(TTY_ID, "PID(%d) get contents from pipe id %d: %s\n", GetPid(), pipe_id, buf);
    }
 
    Exit(0);
    // Never reached
    return;
}



