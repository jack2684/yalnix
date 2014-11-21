/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "../include/hardware.h"

#define TTY_ID 1
//test int CvarInit(int *cvar idp)
//test int CvarWait(int cvar id, int lock id)
//test int Reclaim(int id)

// A consumer producer test case

#define CAPACITY 8

int resource = 0;
int pipe_id;
char buf[11];

void share_get() {
    if(Custom0(pipe_id) == 0) {
        return;
    }
    //TtyPrintf(TTY_ID, "\tPID(%d) inside %s\n", GetPid(), __func__);
    int readcnt = PipeRead(pipe_id, buf, 10);
    if(!readcnt) {
        TtyPrintf(TTY_ID, "PID(%d): Cannot sync from share!!\n", GetPid());
    }
    resource = atoi(buf);
    resource %= 10;     // Take the latest one
}

void share_set() {
    //TtyPrintf(TTY_ID, "\tPID(%d) inside %s\n", GetPid(), __func__);
    buf[0] = (char)(((int)'0') + resource);
    buf[1] = '\n';
    int writecnt = PipeWrite(pipe_id, buf, 10);
    if(!writecnt) {
        TtyPrintf(TTY_ID, "PID(%d): Cannot sync to share!!\n", GetPid());
    }
}

void consume() {
    //TtyPrintf(TTY_ID, "\tPID(%d) inside %s\n", GetPid(), __func__);
    //share_get();
    resource--;
    //share_set();
}

void produce() {
    //TtyPrintf(TTY_ID, "\tPID(%d) inside %s\n", GetPid(), __func__);
    //share_get();
    resource++;
    //share_set();
}

void main(int argc, char **argv) 
{
    user_log("test.cvar program has PID(%d)", GetPid());

    pipe_id = PipeInit();
    //share_set();
	
    int lockid, cvarid;
    LockInit(&lockid);	
	CvarInit(&cvarid);
	int pid = Custom1();
    int cnt = 3;
    
	if(pid == 0){
        pid = GetPid();
        TtyPrintf(TTY_ID, "PID(%d): I am the consumer\n", pid);
        while(cnt--) {
            //TtyPrintf(TTY_ID, "PID(%d): about to acquire lock %d\n", pid, lockid);
            Acquire(lockid);
            TtyPrintf(TTY_ID, "PID(%d): current resource %d\n", pid, resource);
            while(resource <= 0) {
                TtyPrintf(TTY_ID, "PID(%d): wait\n", pid);
                CvarWait(cvarid, lockid); 
                //share_get();
                TtyPrintf(TTY_ID, "PID(%d): resource after wake %d\n", pid, resource);
            }
            TtyPrintf(TTY_ID, "PID(%d): consume \n", pid);
            consume();
            TtyPrintf(TTY_ID, "PID(%d): resource after consume %d\n", pid, resource);
            CvarSignal(cvarid);
            //TtyPrintf(TTY_ID, "PID(%d): signal done\n", pid);
            Release(lockid);
            //TtyPrintf(TTY_ID, "PID(%d): release lock %d\n", pid, lockid);
        }
		Exit(0);
	}
	else{
        pid = GetPid();
        TtyPrintf(TTY_ID, "PID(%d): I am the producer\n", pid);
        while(cnt--) {
            //TtyPrintf(TTY_ID, "PID(%d): about to acquire lock %d\n", pid, lockid);
            Acquire(lockid);
            TtyPrintf(TTY_ID, "PID(%d): current resource %d\n", pid, resource);
            while(resource >= CAPACITY) {
                TtyPrintf(TTY_ID, "PID(%d): no room for new resrouce, wait%d\n", pid, resource);
                CvarWait(cvarid, lockid);      
                share_get();
                TtyPrintf(TTY_ID, "PID(%d): resource after wake%d\n", pid, resource);
            }
            TtyPrintf(TTY_ID, "PID(%d): produce \n", pid);
            produce();
            TtyPrintf(TTY_ID, "PID(%d): resource after produce %d\n", pid, resource);
            CvarSignal(cvarid);
            //TtyPrintf(TTY_ID, "PID(%d): signal done\n", pid);
            Release(lockid);
            //TtyPrintf(TTY_ID, "PID(%d): release lock %d\n", pid, lockid);
        }
	}

    int status;
    TtyPrintf(TTY_ID, "PID(%d): Wait for my children\n", pid);
    Wait(&status);
    TtyPrintf(TTY_ID, "Cvar test done\n");
	Exit(0);
	return;
}
