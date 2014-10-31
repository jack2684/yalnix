#include "common.h"
void main(void) {
    int cnt = 5;
    while(cnt) {
        user_log("GJJ PID(%d) is doing EXEC like a boss, with %d remaining loops", GetPid(), cnt--);
        Pause();
    }
    user_log("GJJ PID(%d) is exiting", GetPid());
    Exit(42);
}
