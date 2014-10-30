#include "debug.h"

void main(void) {
    int delay = 5;
    int delays[512];
    int delays2[1024];
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    int cnt = 0;
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    user_log("running PID(%d)", GetPid());
    
    int pid = Fork();
    char* tmp[] = {NULL};
    
    user_log("After fork come with pid %d", pid);
    if(pid != 0) {
        while(1) {
            user_log("I am parent with PID(%d)", GetPid());
            Exec("src/idle", tmp);
            user_log("PID(%d) try exec fail", GetPid());
            //Delay(3);
            //user_log("PID(%d) back from delay", GetPid());
            Delay(3);
            Pause();
        }
    } else {
        while(1) {
            user_log("I am child with PID(%d)", GetPid());
            //Delay(5);
            //user_log("PID(%d) back from delay", GetPid());
            Delay(1);
            Pause();
        }
    }
    return;
}


