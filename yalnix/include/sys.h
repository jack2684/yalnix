#ifndef _SYS_H
#define _SYS_H
#include "common.h"
#include "timer.h"
#include "hardware.h"

int Y_Delay(UserContext *user_context);
int Y_GetPid(void);
int Y_Brk(void * addr);
int Y_Exit(UserContext *user_context);
int Y_Fork(void);
int Y_Exec(char * filename, char* argvec[]);
int Y_Wait(int * status_ptr);
int Y_WaitPid(int pid, int* status_ptr, int options);


#endif

