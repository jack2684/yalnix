/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "stdlib.h"
#include "../include/hardware.h"

#define TTY_ID 2

int global = 10;

void main(int argc, char **argv) {
    user_log("test.shareframefork program has PID(%d)", GetPid());
    char buf[100];

    int pid = Custom1();
    int local = 10;
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    while(1) {
        if(pid != 0) {
            global++;
            local++;
            TtyPrintf(TTY_ID, "PID(%d) increment: global %d, local %d\n", GetPid(), global, local);
        } else {
            int rnd = rand() % 50;
            if(rnd < 12) {
                global--;
                local--;
                TtyPrintf(TTY_ID, "PID(%d) decrement: global %d, local %d\n", GetPid(), global, local);
            }
            if(local == 0) {
                TtyPrintf(TTY_ID, "PID(%d) local is %d, going to exit\n", GetPid(), local);
                Exit(0);
            }
        }
        Pause();
    }

    Exit(0);

    // Never reached
    return;
}



