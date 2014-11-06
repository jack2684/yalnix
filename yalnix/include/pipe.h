/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _PIPE_H
#define _PIPE_H

#include "hardware.h"
#include "yalnix.h"
#include "dlist.h"
#include "inthashmap.h"
#include "memory.h"
#include "proc.h"
#include "aux.h"

#define MAX_PIPES       1024
#define DEFAULT_LEN     1024

#define RIDX(pipe) (pipe->read_idx % pipe->len)
#define WIDX(pipe) (pipe->write_idx % pipe->len)
#define LEN(pipe) (pipe->len)

typedef struct PIPE {
    int     id;
    char    *buff;
    int     len;
    int     read_idx;       // Read index
    int     write_idx;      // Write index, noted that buff is empty if read_idx == write_idx

    dlist_t *read_queue;    // Typically, pipe have two processes at each side respectively,
    dlist_t *write_queue;   // this queue is just used for whehter a process is waiting here,
                            // though it can be extended to pipe with more than two processes
} pipe_t;

extern hashmap_t *pipe_idp;
extern dlist_t *pipe_id_list;

pipe_t *pipe_init();
int pipe_read(pipe_t *pipe, char *buff, int len, UserContext *user_context);
int pipe_write(pipe_t *pipe, char *buff, int len, UserContext *user_context);
int pipe_enqueue(dlist_t *queue, pcb_t *proc);
pcb_t *pipe_dequeue(dlist_t *queue);

// Internal helper funcitons
int get_next_pipe_id();
int get_buff_size(pipe_t *pipe);
#endif

