#include "tty.h"

dlist_t  *tty_trans_queues[NUM_TERMINALS];
dlist_t  *tty_read_queues[NUM_TERMINALS];

pcb_t   *tty_writing_procs[NUM_TERMINALS];    
pcb_t   *tty_reading_procs[NUM_TERMINALS];

void init_tty()
{
    int i;
    for(i = 0; i < NUM_TERMINALS; i++)
    {
        tty_trans_queues[i] = dlist_init();
        tty_read_queues[i] = dlist_init();
    }
}

//These methods are for tty queues
void tty_trans_enqueue(pcb_t *pcb, unsigned int tty_id)
{       
        //enqueue one to tty-writing-procs queue
        proc_enqueue(tty_trans_queues[tty_id], pcb);
}

void tty_trans_wake_up(unsigned int tty_id)
{       
        pcb_t *pcb;
        
        pcb = tty_trans_dequeue(tty_id);
        if (pcb) {
                //pcb->state = WAIT;
                en_ready_queue(pcb);
        }
        return;
}

pcb_t *tty_trans_dequeue(unsigned int tty_id){
	//dequeue from the tty_writing_procs queue
    return proc_dequeue(tty_trans_queues[tty_id]);
}

void tty_read_enqueue(pcb_t *pcb, unsigned int tty_id)
{
        //enqueue one to tty-reading-procs queue
        proc_enqueue(tty_read_queues[tty_id], pcb);
}

void tty_read_wake_up(unsigned int tty_id)
{
        pcb_t *pcb;
        int ret;

        pcb = tty_read_dequeue(tty_id);
        if (pcb) {
                //pcb ->state = WAIT;
                en_ready_queue(pcb);
        }
        return;
}

pcb_t *tty_read_dequeue(unsigned int tty_id)
{
        //dequeue from the tty_reading_procs queue
        return proc_dequeue(tty_read_queues[tty_id]);
}


//These methods for traps handlers
void tty_reading_wake_up(unsigned int tty_id)
{
        pcb_t *pcb;
        int result;

        pcb = tty_reading_procs[tty_id];
        result = TtyReceive(tty_id, pcb -> tty_buf, pcb -> exit_code);
        pcb -> exit_code = result;
        pcb_wake_up(pcb);

        return;
}

void tty_read_next_ready(unsigned int tty_id)
{
        pcb_t *pcb;
        ///int result;

        pcb = tty_read_dequeue(tty_id);
        if (pcb) {
                //pcb -> state = READY;
                //put at the head of ready queue so next schedule will swtich to it
                proc_enqueue_head(ready_queue, pcb);
        }

        return;
}

void pcb_wake_up(pcb_t *pcb)
{
        en_ready_queue(pcb);
} 

void tty_trans_next_ready(unsigned int tty_id)
{
        pcb_t *pcb;

        pcb = tty_trans_dequeue(tty_id);
        if (pcb) {
                //pcb -> state = READY;
                //put at the head of ready queue so next schedule will swtich to it
                proc_enqueue_head(ready_queue, pcb);
        }

        return;
}

//These two enqueue methods differ in the location of insertion, and set the queue to insert
//default is the ready_queue
int proc_enqueue(dlist_t *queue, pcb_t *proc)
{
        dnode_t *n = dlist_add_tail(queue, proc);
    if(!n) {
        _debug("Cannot enqueue the queue\n");
        return 1;
    }
    log_info("Enqueue with PID(%d)", proc->pid);
    proc->list_node = n; 
    proc->state = READY;
    return 0;
}

int proc_enqueue_head(dlist_t *queue, pcb_t *proc)
{
        dnode_t *n = dlist_add_head(queue, proc);
        if(!n) {
        _debug("Cannot enqueue insert the queue\n");
        return 1;
    }
    log_info("Enqueue insert with PID(%d)", proc->pid);
    proc->list_node = n; 
    proc->state = READY;
    return 0;
}

pcb_t* proc_dequeue(dlist_t *queue)
{
    pcb_t *one_to_go = dlist_rm_head(ready_queue);
    if(one_to_go == NULL) {
        log_info("Ready queue is empty, suck it.");
        return NULL;
    }
    return one_to_go;
}


