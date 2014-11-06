/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "hardware.h"

void main(int argc, char **argv) {
    int delay = 1;
    int delays[512];
    int delays2[1024];
    int exit_status;
    int* a;
    char buf[TERMINAL_MAX_LINE + 2];

    a = (int*)malloc(sizeof(int) * 100);
    int cnt = 0;
    user_log("Init program has PID(%d)", GetPid());
    
    int pid = Fork();
    char* tmp[] = {NULL};
   
    if(argc >= 1) {
        delay = atoi(argv[0]);
    }

    user_log("My pid is %d, fork return pid %d", GetPid(), pid);
    if(pid != 0) {
        user_log("I am parent with PID(%d), user Wait() for my children...", GetPid());
	    //while(1) {
		//TtyPrintf(2,"PARENT using tty to print something\n");
		//test the tty read
		//int test = TtyRead(2, buf, sizeof(buf));
		//TtyPrintf(2,"You have just entered:%s\n",buf);
	    //}
        //int cpid = Wait(&exit_status);
        //user_log("Wait my child(%d) done, return meaning of life %d", cpid, exit_status);
        while(1) {
            TtyPrintf(1,"1111111111111111111111111111110\n");
            //user_log("I have no children, so lonley, going to delay %d seconds", delay);
            //Delay(delay);
        }
    } else {
        while(1) {
            user_log("I am child with PID(%d), about to exe", GetPid());
            while(1) {
		    TtyPrintf(1,"2222222222222222222222222222223\n");
            }
            //Exec("src/goexec", tmp);
            //user_log("PID(%d) try exec fail", GetPid());
            Exit(1);
        }
    }
 
    // Never reached
    return;
}



