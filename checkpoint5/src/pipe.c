/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "pipe.h"

hashmap_t *pipe_idp = NULL;
dlist_t *pipe_id_list = NULL;

pipe_t *pipe_init() {
    if(pipe_idp == NULL) {
        pipe_idp = hashmap_init();
        if(pipe_idp == NULL) {
            log_err("Cannot init hash map pipe_idp");
            return NULL;
        }
    }
    pipe_t *pipe = (pipe_t*)malloc(sizeof(pipe_t));
    if(pipe == NULL) {
        log_err("Cannot create new pipe using malloc");
        return NULL;
    }
    bzero(pipe, sizeof(pipe_t));

    pipe->id = get_next_pipe_id();
    pipe->len = DEFAULT_LEN;
    pipe->read_idx = 0;
    pipe->write_idx = 0;
    pipe->buff = (char*) malloc(sizeof(char) * DEFAULT_LEN);
    if(pipe->buff == NULL) {
        log_err("Cannot init buff in pipe");
        return NULL;
    }
    pipe->read_queue = dlist_init();
    pipe->write_queue = dlist_init();
    if(pipe->read_queue == NULL || pipe->write_queue == NULL) {
        log_err("Cannot init read/write queue in pipe");
        return NULL;
    }

    log_info("Pipe %d is going to pushed into hashmap pipe_idp %p", pipe->id, pipe_idp);
    hashmap_put(pipe_idp, pipe->id, pipe); 
    return pipe;
}

int pipe_read(pipe_t *pipe, char *buff, int len, UserContext *user_context){
    int rc = 0;
    log_info("inside %s, with pipe %p", __func__, pipe);
    if(len <= 0) {
        log_err("pipe_read non positive len %d", len);
        return 1;
    }
    log_info("Validate input done");
    if(len > LEN(pipe)) {
        log_err("pipe_read excceed buff len %d", len);
        return 1;
    }

    log_info("Reading from pipe %d", pipe->id);

    // Block if I don't get what I want, MESA style
    while(get_buff_size(pipe) < len) {
        log_info("Not enough to read, going to wait for a while");
        rc = pipe_enqueue(pipe->read_queue, running_proc);
        if(rc) {
            log_err("Cannot enqueue the pipe read queue");
            return 1;
        }
        next_schedule(user_context);
    }

    log_info("Got enough stuff to read");

    // Read from buff
    if(len + RIDX(pipe) < LEN(pipe)) {
        memcpy(buff, pipe->buff + RIDX(pipe), len);
    } else {
        int first_len = LEN(pipe) - RIDX(pipe);
        int second_len = len - first_len;
        memcpy(buff, pipe->buff + RIDX(pipe), first_len);
        memcpy(buff + first_len, pipe->buff, second_len);
    }
    
    log_info("Read done");

    // Update pipe state
    pipe->read_idx += len;
    if(pipe->read_idx > LEN(pipe)) {
        pipe->read_idx -= LEN(pipe);
        pipe->write_idx -= LEN(pipe);
    }
    
    log_info("Set pointers done");

    // Try wake up writer, if any
    if(pipe->write_queue->size) {
        pcb_t *write_proc = pipe_dequeue(pipe->write_queue);
        en_ready_queue(write_proc);
    }
    log_info("Wake up done");
    return 0;
}

int pipe_write(pipe_t *pipe, char *buff, int len, UserContext *user_context){
    int rc = 0;
    log_info("inside %s, with pipe %p", __func__, pipe);
    if(len <= 0) {
        log_err("pipe_write non positive len %d", len);
        return 1;
    }
    if(len > LEN(pipe)) {
        log_err("pipe_write excceed buff len %d", len);
        return 1;
    }

    log_info("PID(%d) about to write: %s", running_proc->pid, buff);
    // Block if no more space for writing, MESA style
    while(get_buff_size(pipe) + len > LEN(pipe)) {
        log_info("Not enough to write, going to wait for a while, buf size %d, wlen %d, blen %d", get_buff_size(pipe), len, LEN(pipe));
        rc = pipe_enqueue(pipe->write_queue, running_proc);
        if(rc) {
            log_err("Cannot enqueue the pipe write queue");
            return 1;
        }
        next_schedule(user_context);
    }
    
    // Write to buff
    if(WIDX(pipe) + len < LEN(pipe)) {
        memcpy(pipe->buff + WIDX(pipe), buff, len);
    } else {
        int first_len = LEN(pipe) - WIDX(pipe);
        int second_len = len - first_len;
        memcpy(pipe->buff + WIDX(pipe), buff, first_len);
        memcpy(pipe->buff, buff + first_len, second_len);
    }

    // Update pipe states
    pipe->write_idx += len;

    // Try wake up reader, if any
    if(pipe->read_queue->size) {
        pcb_t *read_proc = pipe_dequeue(pipe->read_queue);
        en_ready_queue(read_proc);
    }

    return 0;
}

int pipe_enqueue(dlist_t *queue, pcb_t *proc) {
    if(queue == NULL || proc == NULL) {
        log_err("queue(%p) or proc(%p) pointer is null", queue, proc);
        return 1;
    }

    dnode_t *n = dlist_add_tail(queue, proc);
    if(!n) {
        log_err("Not able to add PID(%d) to queue", proc->pid);
        return 1;
    }
    proc->list_node = n;
    proc->state = WAIT;
    return 0;
}

pcb_t *pipe_dequeue(dlist_t *queue) {
    pcb_t *proc = (pcb_t*)dlist_rm_head(queue);
    if(proc == NULL) {
        log_err("Cannot get any proc from this pipe queue");
    }
    return proc;
}

int get_buff_size(pipe_t *pipe) {
    int size = ((pipe->write_idx + pipe->len) - pipe->read_idx) % pipe->len;
    if(size > pipe->len) {
        log_err("Pipe buff size overflows for some reason, %d > %d", size, pipe->len);
        return -1;
    }
    return size;
}

int get_next_pipe_id() {
    int i;
    if(pipe_id_list == NULL) {
        pipe_id_list = id_generator_init(MAX_PIPES);
    }

    return id_generator_pop(pipe_id_list);
}

int RIDX(pipe_t *pipe) {
    return pipe->read_idx % pipe->len;
}

int WIDX(pipe_t *pipe) {
    return pipe->write_idx % pipe->len;
}

int LEN(pipe_t *pipe) {
    return pipe->len;
}


