/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "hardware.h"

#define TEST_CNT 7

int do_test(int i ) {
    return 0 
        //|| i == 0
        || i == 1
        || i == 2
        || i == 3
        //|| i == 4
        //|| i == 5
        //|| i == 6
        ;
}

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
    user_log("Init program has PID(%d)", GetPid(123, NULL, 123));

    /* Begin test programs
     */
    int pid, i;
    char* empty_argc[] = {NULL};
    char **tests = (char**)malloc(sizeof(char*) * TEST_CNT);
    tests[0] = "src/test.tty";
    tests[1] = "src/test.pipe";
    tests[2] = "src/test.lock";
    tests[3] = "src/test.cvar";
    tests[4] = "src/test.bigstack";
    for(i = 0; i < TEST_CNT; i++) {
        if(!do_test(i)) {
            continue;
        }
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

