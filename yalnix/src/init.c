#include "debug.h"

void main(void) {
    int delay = 5;
    int delays[512];
    int delays2[1024];
    int exit_status;
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    int cnt = 0;
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    user_log("running PID(%d)", GetPid());
    
    int pid = Fork();
    char* tmp[] = {NULL};
    
    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        user_log("I am parent with PID(%d), user Wait() for my children...", GetPid());
        int cpid = Wait(&exit_status);
        user_log("Wait my child(%d) done, return meaning of life %d", cpid, exit_status);
        while(1) {
            user_log("I have no children, so lonley");
            Delay(3);
        }
    } else {
        while(1) {
            user_log("I am child with PID(%d), about to exe", GetPid());
            Exec("src/goexec", tmp);
            user_log("PID(%d) try exec fail", GetPid());
            Exit(1);
        }
    }
    return;
}


