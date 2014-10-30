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
    
    user_log("After fork come with pid %d", pid);
    if(pid != 0) {
        user_log("I am parent with PID(%d)", GetPid());
        int cpid = Wait(&exit_status);
        user_log("Wait my child(%d) done, return meaning of life %d", cpid, exit_status);
        while(1) {
            //Delay(3);
            //user_log("PID(%d) back from delay", GetPid());
            Delay(3);
            Pause();
        }
    } else {
        while(1) {
            user_log("I am child with PID(%d), about to exe", GetPid());
            Exec("src/goexec", tmp);
            user_log("PID(%d) try exec fail", GetPid());
            //Delay(5);
            //user_log("PID(%d) back from delay", GetPid());
            Delay(1);
            Pause();
        }
    }
    return;
}


