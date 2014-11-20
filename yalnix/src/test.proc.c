/* Team 3: stderr, Junjie Guan, Ziyang Wang*/

#include "hardware.h"
#include "common.h"
#include "stdlib.h"
#include "dlist.h"
void main(void) {
    int pid = GetPid(), cpid = -1;
    int idp, cnt = 0, stat;
    int *a;
    while(1) {
        // Parent randomly generate new process
        if(pid == GetPid() && percent_chance(50)) {
            cpid = Fork();
            if(cpid != 0) {
                cnt++;
                TtyPrintf(0, "Create child pid %d, now child cnt is %d\n", cpid, cnt);
                if(cnt == 10) {
                    int i;
                    for(i = 0; i < cnt; i++) {
                        TtyPrintf(0, "Wait for %dth child\n", i);
                        Wait(&stat);
                    }
                    cnt = 0;
                }
            } else {
                a = (int*)malloc(PAGESIZE * 50);     
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
                TtyPrintf(0, "Child %d now exiting\n", GetPid());
                free(a);
                Exit(0);
            } 
            int delay = rand() % 5;
            Delay(delay);
        }
    }
    return;
}

int percent_chance(int percent) {
    int rnd = rand() % 100;
    //TtyPrintf(0, "Rnd is %d\n", rnd);
    return rnd < percent;
}

