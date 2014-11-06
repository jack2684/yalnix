/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 1

void main(int argc, char **argv) {
    user_log("test.tty program has PID(%d)", GetPid());
    TtyPrintf(TTY_ID, "Testing TTY on terminal %d, while TERMINAL_MAX_LINE is %d\n", TTY_ID, TERMINAL_MAX_LINE);
    int cnt = 5;
    char buf[TERMINAL_MAX_LINE + 2];
    int pid = Fork();
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        user_log("I am parent with PID(%d), user Wait() for my children...", GetPid());
        while(cnt--) {
            TtyPrintf(TTY_ID, "(%d+++++++++)", cnt);
        }
        TtyPrintf(1, "\n");
    } else {
        user_log("I am child with PID(%d), about to exe", GetPid());
        while(cnt--) {
            TtyPrintf(TTY_ID, "<%d-------->", cnt);
        }
        TtyPrintf(TTY_ID, "\n");
        Delay(5);   // Delay to avoid overlap printing
    }
 
    TtyPrintf(TTY_ID, "Enter something so that PID(%d) can read from terminal:\n", GetPid());
    TtyRead(TTY_ID, buf, 2);
    if(pid == 0) {
        Delay(3);
    }
    TtyPrintf(TTY_ID, "PID(%d) get: %s\n", GetPid(), buf);
    
    Exit(0);
    // Never reached
    return;
}



