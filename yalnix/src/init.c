void main(void) {
    int delay = 11;
    int delays[512];
    int delays2[1024];
    //int delays3[1024 * 2];
    //int delays4[1024 * 4];
    //int delays5[1024 * 8];
    TracePrintf(1, "GJJ hahahah Inside init proc, going to work %d seconds\n", delay);
    Delay(delay);
    TracePrintf(1, "GJJ leaving the init proc\n");
    return;
}


