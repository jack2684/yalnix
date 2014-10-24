void main(void) {
    int delay = 10;
    int delays[512];
    int delays2[1024];
    int* a;
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    a = (int*)malloc(sizeof(int) * 100);
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    while(1) {
        TracePrintf(1, "Hahahah Inside init proc, PID %d, going to sleep %d seconds\n", GetPid(), delay);
        Delay(delay);
        Pause();
    }
    return;
}


