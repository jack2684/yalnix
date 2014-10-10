//All these syscalls are named with the prefix "Y_"

//These are process syscalls
int Y_Fork(void);

int Y_Exec(char * filename, char* argvec[]);

int Y_Exit(int status);

int Y_Wait(int * status_ptr);

int Y_WaitPid(int pid, int* status_ptr, int options);

int Y_GetPid(void);

int Y_Brk(void * addr);
 
int Y_Delay(int clock_ticks);

//These are tty syscalls
int Y_TtyRead(int tty_id, void *buf, int len);

int Y_TtyWrite(int tty_id, void *buf, int len);

//These are Pipe syscalls
int Y_PipeInit(int *pipe_idp);

int Y_PipeRead(int pipe_id, void *buf, int len);

int Y_PipeWrite(int pipe_id, void *buf, int len);

//These are lock syscalls
int Y_LockInit(int *lock_idp);

int Y_Acquire(int lock_id);

int Y_Release(int lock_id);

//These are condition variables syscalls
int Y_CvarInit(int *cvar_idp);

int Y_CvarSignal(int cvar_id);

int Y_CvarWait(int cvar_id, int lock_id);

int Y_CvarBroadcast(int cvar_id);

//These are destroy syscalls
int Y_Reclaim(int id);

//Here are some extra syscalls below that could help implement some extra functions
//We will complete them if possible

//These are 3 custom syscalls
int Y_Custom0();

int Y_Custom1();

int Y_Custom2();

//These are semaphore syscalls
int Y_SemInit(int *, int);

int Y_SemUp (int);

int Y_SemDown (int);

int Y_Nop(int, int, int, int);

int Y_Boot();

int Y_Register(unsigned int **);

int Y_Send(void* , int);

int Y_Receive(void *){

int Y_ReceiveSpecific(void *, int ){

int Y_Reply (void *, int){

int Y_Forward (void *, int, int){

int Y_CopyFrom (int, void *, void *, int){

int Y_CopyTo (int, void *, void *, int){

int Y_ReadSector (int, void *){

int Y_WriteSector (int, void *){


