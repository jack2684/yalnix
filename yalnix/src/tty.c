#include "tty.h"

dlist_t  *tty_write_queues[NUM_TERMINALS];
dlist_t  *tty_read_queues[NUM_TERMINALS];

pcb_t   *tty_writing_procs[NUM_TERMINALS];    
pcb_t   *tty_reading_procs[NUM_TERMINALS];

void init_tty()
{
    int i;
    for(i = 0; i < NUM_TERMINALS; i++)
    {
        tty_write_queues[i] = dlist_init();
        tty_read_queues[i] = dlist_init();
        tty_writing_procs[i] = NULL;
        tty_reading_procs[i] = NULL;
    }
}

//These methods are for tty queues
void tty_write_enqueue(pcb_t *pcb, unsigned int tty_id)
{       
        //enqueue one to tty-writing-procs queue
        tty_proc_enqueue(tty_write_queues[tty_id], pcb);
}

void tty_write_wake_up(unsigned int tty_id)
{       
        pcb_t *pcb;
        
        pcb = tty_write_dequeue(tty_id);
        if (pcb) {
                //pcb->state = WAIT;
                en_ready_queue(pcb);
        }
        return;
}

pcb_t *tty_write_dequeue(unsigned int tty_id){
	//dequeue from the tty_writing_procs queue
    return tty_proc_dequeue(tty_write_queues[tty_id]);
}

void tty_read_enqueue(pcb_t *pcb, unsigned int tty_id)
{
        //enqueue one to tty-reading-procs queue
        tty_proc_enqueue(tty_read_queues[tty_id], pcb);
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
        return tty_proc_dequeue(tty_read_queues[tty_id]);
}


//These methods for traps handlers
void tty_reading_wake_up(unsigned int tty_id)
{
        pcb_t *pcb;
        int chars_get;

        pcb = tty_reading_procs[tty_id];
        chars_get = TtyReceive(tty_id, pcb -> tty_buf, pcb -> exit_code);
        pcb -> exit_code = chars_get;
        //pcb_wake_up(pcb);

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
                en_ready_queue(pcb);
        }

        return;
}

void pcb_wake_up(pcb_t *pcb)
{
        en_ready_queue(pcb);
} 

void tty_write_next_ready(unsigned int tty_id)
{
        pcb_t *pcb;

        pcb = tty_write_dequeue(tty_id);
        if (pcb) {
                //writemit finish, dequeue head from tty_write_queue and enqueue into ready queue
                en_ready_queue(pcb);
        }

        return;
}


int tty_proc_enqueue(dlist_t *tty_queue, pcb_t *proc)
{
    //first check if proc is already in the queue
    log_info("Before tty enqueue with PID(%d), now queue size %d", proc->pid, tty_queue->size);
    if(!tty_queue){
        return 0;
    }

    //not in the queue, add to the tail
    dnode_t *n = dlist_add_tail(tty_queue, proc);
    if(!n) 
    {
        log_err("Cannot enqueue the tty queue\n");
        return 1;
    }
    log_info("Tty enqueue with PID(%d), now queue size %d", proc->pid, tty_queue->size);
    proc->list_node = n; 
    proc->state = WAIT;
    return 0;
}

pcb_t* tty_proc_dequeue(dlist_t *tty_queue)
{
    log_info("Before tty dequeue, queue size %d", tty_queue->size);
    pcb_t *one_to_go = dlist_rm_head(tty_queue);
    if(one_to_go == NULL) {
        log_info("Nothing to dequeue");
        return NULL;
    }
    log_info("Tty dequeue with PID(%d), size: %d", one_to_go->pid, tty_queue->size);
    return one_to_go;
}

int is_tty_write_head(pcb_t *proc, int tty_id) {
    pcb_t *hproc = (pcb_t*) dlist_peek_head(tty_write_queues[tty_id]);
    return hproc == proc;
}

int is_tty_read_head(pcb_t *proc, int tty_id){
    pcb_t *hproc = (pcb_t*) dlist_peek_head(tty_read_queues[tty_id]);
    return hproc == proc;
}

pcb_t *peek_tty_write_queue(int tty_id) {
    dnode_t *node = tty_write_queues[tty_id]->head;
    if(!node) {
        return NULL;
    } else {
        return (pcb_t*) node->data;
    }
}

pcb_t *peek_tty_read_queue(int tty_id) {
    dnode_t *node = tty_read_queues[tty_id]->head;
    if(!node) {
        return NULL;
    } else {
        return (pcb_t*) node->data;
    }
}

int is_write_busy(int tty_id) {
    return tty_writing_procs[tty_id] != NULL;
}

void set_write_proc(pcb_t *proc, int tty_id) {
    tty_writing_procs[tty_id] = proc;
}

void set_read_proc(pcb_t *proc, int tty_id) {
    tty_reading_procs[tty_id] = proc;
}

