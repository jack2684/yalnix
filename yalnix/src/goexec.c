void main(void) {
    int cnt = 5;
    while(cnt) {
        TracePrintf(5, "GJJ PID(%d) is doing EXEC like a boss, with %d remaining loops\n", GetPid(), cnt--);
        Pause();
    }
    TracePrintf(5, "GJJ PID(%d) is exiting\n", GetPid());
    Exit(42);
}
