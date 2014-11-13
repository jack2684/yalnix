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
    if(pipe->id == -1) {
        log_err("Cannot get new pipe id");
        free(pipe);
        return NULL;
    }
    pipe->len = DEFAULT_LEN;
    pipe->read_idx = 0;
    pipe->write_idx = 0;
    pipe->buff = (char*) malloc(sizeof(char) * DEFAULT_LEN);
    if(pipe->buff == NULL) {
        log_err("Cannot init buff in pipe");
        free(pipe->buff);
        free(pipe);
        return NULL;
    }
    pipe->read_queue = dlist_init();
    pipe->write_queue = dlist_init();
    if(pipe->read_queue == NULL || pipe->write_queue == NULL) {
        log_err("Cannot init read/write queue in pipe");
        free(pipe->buff);
        free(pipe);
        return NULL;
    }

    log_info("Pipe %d is going to pushed into hashmap pipe_idp %p", pipe->id, pipe_idp);
    hashmap_put(pipe_idp, pipe->id, pipe); 
    return pipe;
}

int pipe_read(pipe_t *pipe, char *buff, int len, UserContext *user_context){
    int rc = 0, read_cnt = 0;
    log_info("inside %s, with pipe %p", __func__, pipe);
    if(len <= 0) {
        log_err("pipe_read non positive len %d", len);
        return -1;
    }
    //if(len > LEN(pipe)) {
    //    log_err("pipe_read excceed buff len %d", len);
    //    return -1;
    //}

    log_info("Reading from pipe %d", pipe->id);

    // Try to read from buff as much as possible
    while(len) {
        int buff_size = get_buff_size(pipe);
        int this_len = len > buff_size ? buff_size : len;
        read_this_much(pipe, buff + read_cnt, this_len);
        read_cnt += this_len;
        len -= this_len;
        try_wake_up_writer(pipe);
        log_info("Remaining len: %d", len);
        if(len != 0) { // Block and wait for new incoming data
            block_reader(pipe, user_context); 
        }
    }

    return read_cnt;
}

void try_wake_up_writer(pipe_t *pipe) {
    // Try wake up writer, if any
    if(pipe->write_queue->size) {
        pcb_t *write_proc = proc_dequeue(pipe->write_queue);
        en_ready_queue(write_proc);
    }
}

void read_this_much(pipe_t *pipe, char *buff, int len) {
    if(len == 0) {
        return;
    }
    // Read from buff
    if(len + RIDX(pipe) < LEN(pipe)) {
        memcpy(buff, pipe->buff + RIDX(pipe), len);
    } else {
        int first_len = LEN(pipe) - RIDX(pipe);
        int second_len = len - first_len;
        memcpy(buff, pipe->buff + RIDX(pipe), first_len);
        memcpy(buff + first_len, pipe->buff, second_len);
    }
    
    // Update pipe state
    pipe->read_idx += len;
    if(pipe->read_idx >= LEN(pipe)) {
        pipe->read_idx -= LEN(pipe);
        pipe->write_idx -= LEN(pipe);
    }
    
    log_info("%d chars are read", len);
}

int pipe_write(pipe_t *pipe, char *buff, int len, UserContext *user_context){
    int rc = 0, write_cnt = 0;
    log_info("inside %s, with pipe %p", __func__, pipe, len);
    if(len <= 0) {
        log_err("pipe_write non positive len %d", len);
        return -1;
    }
    //if(len > LEN(pipe)) {
    //    log_err("pipe_write excceed buff len %d", len);
    //    return -1;
    //}

    log_info("PID(%d) about to write: %s, len: %d, input len: %d", running_proc->pid, buff, strlen(buff), len);
    while(len) {
        int empty_buff_size = LEN(pipe) - get_buff_size(pipe);
        int this_len = len > empty_buff_size ? empty_buff_size : len;
        write_this_much(pipe, buff + write_cnt, this_len);
        write_cnt += this_len;
        len -= this_len;
        try_wake_up_reader(pipe);
        log_info("Remaining len: %d", len);
        if(len != 0) { // Block and wait for new incoming data
            block_writer(pipe, user_context); 
        }
    }

    return write_cnt;
}

void try_wake_up_reader(pipe_t *pipe) {
    // Try wake up reader, if any
   if(pipe->read_queue->size) {
        pcb_t *read_proc = proc_dequeue(pipe->read_queue);
        en_ready_queue(read_proc);
    }
}

void write_this_much(pipe_t *pipe, char *buff, int len) {
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
    log_info("%d chars are writen", len);
}

int block_reader(pipe_t *pipe, UserContext *user_context) {
    log_info("Not enough to read, going to wait for a while");
    int rc = proc_enqueue(pipe->read_queue, running_proc);
    if(rc) {
        log_err("Cannot enqueue the pipe read queue");
        return 1;
    }
    next_schedule(user_context);
    return 0;
}

int block_writer(pipe_t *pipe, UserContext *user_context) {
    log_info("Not enough to write, going to wait for a while");
    int rc = proc_enqueue(pipe->write_queue, running_proc);
    if(rc) {
        log_err("Cannot enqueue the pipe write queue");
        return 1;
    }
    next_schedule(user_context);
    return 0;
}

int free_pipe(pipe_t *pipe) { 
    int rc = 0, id = pipe->id;
    rc = dlist_destroy(pipe->read_queue);
    if(rc) {
        log_err("Cannot destroy read queue");
        return 1;
    }
    rc = dlist_destroy(pipe->write_queue);
    if(rc) {
        log_err("Cannot destroy write queue");
        return 1;
    }
    free(pipe->buff);
    free(pipe);
    pipe = NULL;
    id_generator_push(pipe_id_list, id);
    log_info("pipe %d is freed", id);
    return 0;
}

int get_buff_size(pipe_t *pipe) {
    log_info("widx: %d, ridx: %d", pipe->write_idx, pipe->read_idx);
    int size = ((pipe->write_idx ) - pipe->read_idx) ;
    if(size > pipe->len) {
        log_err("Pipe buff size overflows for some reason, %d > %d", size, pipe->len);
        return -1;
    }
    log_info("Current buff size is %d", size);
    return size;
}

int get_next_pipe_id() {
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


