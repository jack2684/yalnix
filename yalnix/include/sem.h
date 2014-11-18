/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _SEM_H
#define _SEM_H
#include "yalnix.h"
#include "hardware.h"
#include "common.h"
#include "inthashmap.h"
#include "dlist.h"
#include "proc.h"

typedef struct
{
	int id;
	int value;
	dlist_t *waits;
	pcb_t *owner;
}sem_t;

sem_t *sem_init(int value);
int sem_down(sem_t *sem, UserContext *user_context);
int sem_up(sem_t *sem);
int free_sem(sem_t *sem);

#endif
