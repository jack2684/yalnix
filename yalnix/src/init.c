void main(void) {
    int delay = 5;
    int delays[512];
    int delays2[1024];
    int delays3[1024 * 2];
    int delays4[1024 * 2 * 2];
    int delays5[1024 * 2 * 2 * 2];
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    int cnt = 0;
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    TracePrintf(1, "@@@@@@@@@@@@@@@@@@@@@@@@@@@\t running PID(%d)\n", GetPid());
    
    int pid = Fork();

    
    TracePrintf(1, "@@@@@@@@@@@@@@@@@@@@@@@@@@@\t After fork come with pid %d\n", pid);
    if(pid == 0) {
        while(1) {
            TracePrintf(1, "@@@@@@@@@@@@@@@@@@@@@@@@@@@\t I am child with PID(%d)\n", GetPid());
            Pause();
        }
    } else {
        while(1) {
            TracePrintf(1, "@@@@@@@@@@@@@@@@@@@@@@@@@@@\t I am parent with PID(%d)\n", GetPid());
            Pause();
        }
    }
    return;
}


