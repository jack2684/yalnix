/* Team 3: stderr, Junjie Guan, Ziyang Wang*/

#include "hardware.h"
#include "common.h"
#include "stdlib.h"
#include "dlist.h"
void main(void) {
    int pid = GetPid(), cpid = -1;
    int idp, cnt = 0;
    while(1) {
        // Parent randomly generate new process
        if(pid == GetPid() && percent_chance(50)) {
            cpid = Fork();
            if(cpid != 0) {
                TtyPrintf(0, "Create child pid %d\n", cpid);
                cnt++;
                if(cnt == 10) {
                    int i;
                    for(i = 0; i < cnt; i++) {
                        TtyPrintf(0, "Wait for %dth child\n", i);
                        Wait();
                    }
                }
                cnt = 0;
            }
        }

        // Child randomly exit the process
        if(cpid == 0) {
            if(percent_chance(50)) {
                PipeInit(&idp);
            } 
            if(percent_chance(50)) {
                LockInit(&idp);
            } 
            if(percent_chance(50)) {
                CvarInit(&idp);
            } 
            if(percent_chance(50)) {
                SemInit(&idp);
            } 
            if(percent_chance(50)) {
                Exit(0);
            } 
        }
//        Pause();
    }
    return;
}

int percent_chance(int percent) {
    int rnd = rand() % 100;
    return rnd < percent;
}

