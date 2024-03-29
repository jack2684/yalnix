/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _SYS_H
#define _SYS_H
#include "common.h"
#include "hardware.h"
#include "timer.h"
#include "proc.h"
#include "pipe.h"
#include "tty.h"

#define min(a, b) ( a > b ? b : a)

// Basically if a syscall don't need return code, 
// then we don't need to pass the user context into that call

int Y_Delay(UserContext *user_context);
int Y_GetPid(UserContext *user_context);
int Y_Brk(uint32 addr);
int Y_Exit(int exit_code, UserContext *user_context);
int Y_Fork(UserContext *user_context);
int Y_Exec(char * filename, char* argvec[], UserContext *user_context);
int Y_Wait(int * status_ptr, UserContext *user_context);
int Y_WaitPid(int pid, int* status_ptr, int options);
int Y_TtyWrite(int tty_id, void *buf, int len, UserContext *user_context);
int Y_TtyRead(int tty_id, void *buf, int len, UserContext *user_context);
int Y_PipeInit();
int Y_PipeRead(int pipe_id, void *buf, int len, UserContext *user_context);
int Y_PipeWrite(int pipe_id, void *buf, int len, UserContext *user_context);
#endif

