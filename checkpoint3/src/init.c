void main(void) {
    int delay = 5;
    int delays[512];
    int delays2[1024];
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    int cnt = 0;
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    TracePrintf(1, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Hahahah Inside init proc, PID %d \n");
    while(1) {
        TracePrintf(1, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Inside init proc, PID %d, going to sleep %d seconds, cnt %d\n", GetPid(), delay, cnt++);
        Delay(delay);
        TracePrintf(1, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Init back from sleep\n");
        Pause();
    }
    return;
}


