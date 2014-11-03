/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _PROC_H
#define _PROC_H

#include "hardware.h"
#include "yalnix.h"
#include "memory.h"
#include "dlist.h"

#define MAX_PIPES       1024
#define DEFAULT_LEN     1024

typedef struct PIPE {
    char    *buff;
    int     len;
    int     read_idx;       // Read index
    int     write_idx       // Write index
    dlist   *read_queue;    // Typically, pipe have two processes at each side respectively,
    dlist   *write_queue;   // this queue is just used for whehter a process is waiting here,
                            // though it can be extended to pipe with more than two processes
} pipe_t;

extern dlist_t *pipe_idp;

pipe_t *pipe_init();
int pipe_read(pipe_t *pipe, char *buff, int len, UserContext *user_context);
int pipe_write(pipe_t *pipe, char *buff, int len, UserContext *user_context);

#endif

