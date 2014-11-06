/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "hardware.h"

void main(int argc, char **argv) {
    /* These several lines are just some basic test of 
     *  growing stack and growing heap in user land
     */
    int delay = 1;
    int delays[512];
    int delays2[1024];
    int exit_status;
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    user_log("Init program has PID(%d)", GetPid());

    /* Begin test programs
     */
    int test_cnt = 2, pid, i;
    char* empty_argc[] = {NULL};
    char **tests = (char**)malloc(sizeof(char*) * test_cnt);
    tests[0] = "src/test.tty";
    tests[1] = "src/test.pipe";
    for(i = 0; i < test_cnt - 1; i++) {
        pid= Fork();
        if(pid == 0) {
            user_log("I am child with PID(%d), about to exec program: %s", GetPid(), tests[i]);
            Exec(tests[i], empty_argc);   
            user_log("ERROR: child PID(%d) cannot exec program: %s", GetPid(), tests[i]);
            Exit(1);
        }
    }

    while(1) {
        Pause();
    }
    // Never reached, init program should not exit
    return;
}



