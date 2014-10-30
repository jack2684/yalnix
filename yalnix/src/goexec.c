void main(void) {
    int cnt = 20;
    while(cnt) {
        TracePrintf(5, "GJJ is doing EXEC like a boss, with %d remaining loops\n", cnt--);
        Pause();
    }
    Exit(42);
}
