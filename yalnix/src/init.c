#include "hardware.h"
#include "common.h"

void DoIdle(void) {
    while(1) {
        TracePrintf(1, "DoIdle\n");
        Pause();
    }   
}

void main(void) {
    DoIdle();
}


