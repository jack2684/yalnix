#ifndef _SYS_H
#define _SYS_H
#include "common.h"
#include "timer.h"
#include "hardware.h"

// Basically if a syscall don't need return code, 
// then we don't need to pass the user context into that call

int Y_Delay(UserContext *user_context);
int Y_GetPid(UserContext *user_context);
int Y_Brk(uint32 addr);
int Y_Exit(UserContext *user_context);
int Y_Fork(UserContext *user_context);
int Y_Exec(char * filename, char* argvec[], UserContext *user_context);
int Y_Wait(int * status_ptr);
int Y_WaitPid(int pid, int* status_ptr, int options);

#endif

