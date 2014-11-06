/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "hardware.h"

void main(int argc, char **argv) {
    int delay = 1;
    int delays[512];
    int delays2[1024];
    int exit_status;
    int* a;
    char buf[TERMINAL_MAX_LINE + 2];

    a = (int*)malloc(sizeof(int) * 100);
    user_log("Init program has PID(%d)", GetPid());
    
    int pid = Fork();
    char* tmp[] = {NULL};
   
    if(argc >= 1) {
        delay = atoi(argv[0]);
    }

    int cnt = 5;
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        user_log("I am parent with PID(%d), user Wait() for my children...", GetPid());
        while(cnt--) {
            TtyPrintf(1, "(%d+++++++++++++++)", cnt);
        }
        TtyPrintf(1, "\n");
    } else {
        user_log("I am child with PID(%d), about to exe", GetPid());
        while(cnt--) {
            TtyPrintf(1, "<%d--------------->", cnt);
        }
        TtyPrintf(1, "\n");
        Delay(5);
    }
 
    TtyPrintf(1, "Enter something so that PID(%d) can read from terminal:\n", GetPid());
    TtyRead(1, buf, 2);
    TtyPrintf(1, "PID(%d) get: %s", GetPid(), buf);
    
    if(pid != 0) {
        do {
           Pause();
        } while(1);
    }
    Exit(0);
    // Never reached
    return;
}



