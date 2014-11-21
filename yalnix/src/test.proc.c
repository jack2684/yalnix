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
        // Parent generate new process
        if(pid == GetPid() && percent_chance(50)) {
            user_log("About to fork");
            cpid = Fork();
            user_log("Fork done");
            if(cpid > 0) {
                cnt++;
                TtyPrintf(0, "Create child pid %d, now child cnt is %d\n", cpid, cnt);
                user_log("Might about to wait");
                if(cnt == 10) {
                    int i;
                    for(i = 0; i < cnt; i++) {
                        TtyPrintf(0, "Wait for %dth child\n", i);
                        Wait(&stat);
                    }
                    cnt = 0;
                }
                user_log("Done wait");
            } else if(cpid < 0) {
                user_log("Cannot create child");
                TtyPrintf(0, "Cannot create child, process pool is empty\n");
            }
        }

        // Child randomly exit the process
        if(cpid == 0) {
            a = (int*)malloc(PAGESIZE * 50);     
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

