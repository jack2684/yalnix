/* Team 3: stderr, Junjie Guan, Ziyang Wang*/

#include "hardware.h"
#include "common.h"
#include "stdlib.h"
void main(void) {
    int delay = 4;
    int delays[512];
    int delays2[1024];
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    while(1) {
        //TtyPrintf(3, "PID(%d) about to fork very quickly\n", GetPid());
        int npg = (rand()) % 75;
        user_log("PID(%d) about to malloc %d pages", GetPid(), npg);
        TtyPrintf(3, "PID(%d) about to malloc %d pages to p\n", GetPid(), npg);
        void *p = (void*)malloc(npg * PAGESIZE);
        if(p == NULL) {
            TtyPrintf(3, "PID(%d) end test\n", GetPid());
            while(1)
                Pause();
        }
        TtyPrintf(3, "PID(%d) p got ptr at %p, about to free %d\n", GetPid(), p, npg);
        user_log("PID(%d) about to free %d pages", GetPid(), npg);
        free(p);
        //(1, "GJJ is coding like a boss.\n");
//       Delay(delay);
        Pause();
    }
    TracePrintf(1, "GJJ leaving the init proc\n");
    return;
}


