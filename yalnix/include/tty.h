/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _TTY_H
#define _TTY_H

#include "proc.h"
#include "dlist.h"
#include "hardware.h"

//queues stores the procs waiting to do tty actions
extern dlist_t  *tty_trans_queues[NUM_TERMINALS];
extern dlist_t  *tty_read_queues[NUM_TERMINALS];

//these pcb store the blocking processes waiting to complete the tty action
extern pcb_t   *tty_writing_procs[NUM_TERMINALS];          
extern pcb_t   *tty_reading_procs[NUM_TERMINALS];

extern int trans_finish;

void init_tty();

void tty_read_enqueue(pcb_t *pcb, unsigned int tty_id);

void tty_read_wake_up(unsigned int tty_id);

pcb_t *tty_read_dequeue(unsigned int tty_id);

void tty_trans_enqueue(pcb_t *pcb, unsigned int tty_id);

void tty_trans_wake_up(unsigned int tty_id);

pcb_t* tty_trans_dequeue(unsigned int tty_id);

void tty_reading_wake_up(unsigned int tty_id);

void tty_read_next_ready(unsigned int tty_id);

void pcb_wake_up(pcb_t *pcb);

void tty_trans_next_ready(unsigned int tty_id);

int tty_proc_enqueue(dlist_t *queue, pcb_t *proc);

int tty_proc_enqueue_head(dlist_t *queue, pcb_t *proc);

pcb_t* tty_proc_dequeue(dlist_t *queue);

#endif

