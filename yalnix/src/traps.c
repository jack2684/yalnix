/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
//These are the traps used in Yalnix
//These introduction for each TRAP is from the Yalnix 2014.pdf
#include "traps.h"

int no_traps;
void TrapsInit(){
    
}

char *get_sys_call_name(u_long code) {
    switch(code) {
        case YALNIX_EXIT:
            return "YALNIX_EXIT";
        case YALNIX_GETPID:
            return "YALNIX_GETPID";
        case YALNIX_BRK:
            return "YALNIX_BRK";
        case YALNIX_DELAY:
            return "YALNIX_DELAY";
        case YALNIX_FORK:
            return "YALNIX_FORK";
        case YALNIX_EXEC:
            return "YALNIX_EXEC";
        case YALNIX_WAIT:
            return "YALNIX_WAIT";
        case YALNIX_TTY_READ:
            return "YALNIX_TTY_READ";
        case YALNIX_TTY_WRITE:
            return "YALNIX_TTY_WRITE";
        default:
            break;
    } 
}

/*This trap results from a “kernel call” trap instruction executed by the current user processes. 
Such a trap is used by user processes to request some type of service from the operating system kernel, 
such as creating a new process, allocating memory, or performing I/O. 
All different kernel call requests enter the kernel through this single type of trap*/
void trap_kernel_handler(UserContext *user_context){
    u_long rc = 0;
    u_long code = user_context->code;
    log_info("Sys call: %s, with PID %d at pc %p", get_sys_call_name(code), running_proc->pid, user_context->pc);
    switch(code) {
        case YALNIX_EXIT:
            Y_Exit(user_context->regs[0], user_context);
            break;
        case YALNIX_GETPID:
            user_context->regs[0] = Y_GetPid(user_context);
            break;
        case YALNIX_BRK:
            rc = Y_Brk(user_context->regs[0]);
            break;
        case YALNIX_DELAY:
            rc = Y_Delay(user_context);
            break;
        case YALNIX_FORK:
            user_context->regs[0] = Y_Fork(user_context);
            break;
        case YALNIX_EXEC:
            Y_Exec((char *)user_context->regs[0], (char **)user_context->regs[1], user_context);
            break;
        case YALNIX_WAIT:
            user_context->regs[0] = Y_Wait((int *)user_context->regs[0], user_context);
            break;
        case YALNIX_TTY_READ:
            user_context->regs[0] = Y_TtyRead(user_context->regs[0],
                                                (char *)user_context->regs[1],
                                                user_context->regs[2],
                                                user_context);
            break;
        case YALNIX_TTY_WRITE:
            user_context->regs[0] = Y_TtyWrite(user_context->regs[0], 
                                                (char *)user_context->regs[1], 
                                                user_context->regs[2], 
                                                user_context);
            break;
        default:
            break;
    }

    if(rc) {
        log_err("Kernel call for %s fail!", get_sys_call_name(code));
    }
    return; 
}


/*This exception results from any arithmetic error from an 
instruction executed by the current user process, such as division by zero or an arithmetic overflow.*/
void trap_math_handler(UserContext *user_context){
	//INVOKE the TRAP_MATH_HANDLER
	//KILL the process with the arithmetic error
}

/*This exception results from the execution of an illegal instruction by the currently executing user process. 
An illegal instruction can be an undefined machine lan- guage opcode, an illegal addressing mode, 
or a privileged instruction when not in kernel mode.*/
void trap_illegal_handler(UserContext *user_context){
	//INVOKE the TRAP_ILLEGAL_HANDLER
	//KILL the process executing the illegal instruction
}

/*
This exception results from a disallowed memory access by the current user process. 
The access may be disallowed because the address is outside the virtual address range of the hardware (outside Region 0 and Region 1), 
because the address is not mapped in the current page tables, 
or because the access violates the page protection specified in the corresponding page table entry.
*/
void trap_memory_handler(UserContext *user_context){
    if(running_proc == idle_proc) {
        return;
    }
    uint32 offending_addr = (uint32)user_context->addr;
    int rc = 0;

    switch(user_context->code) {
        case YALNIX_MAPERR:
            log_info("User addr %p map err", offending_addr);
            rc = user_stack_resize(running_proc, offending_addr);
            if(rc) {
                log_err("Cannot resize proc user stack");
                // @TODO: exit process, tmp using halt
                Halt();
            }
            break;
        case YALNIX_ACCERR:
            log_err("User addr %p acc err", offending_addr);
            // @TODO: exit process, tmp using halt
            break;
        default:
            break;
    }
}

//This interrupt results from the machine’s hardware clock, which generates periodic clock interrupts
void trap_clock_handler(UserContext *user_context){
    ticking_down();
    if(round_robin_timeout()) {
        round_robin_schedule(user_context);
    }
}

/*
This interrupt is generated by the terminal device controller hard- ware, 
when a complete line of input is available from one of the terminals attached to the system.
*/
void trap_tty_receive_handler(UserContext *user_context)
{
    unsigned int tty_id = user_context->code;
    int chars_get;

    log_info("Get sth from terminal when read queue size: %d", tty_read_queues[tty_id]->size);
    pcb_t *proc = peek_tty_read_queue(tty_id);
    while(proc != NULL) {
        chars_get = TtyReceive(tty_id, proc->tty_buf, proc->exit_code);
        log_info("PID(%d) gets %d chars from term %d: %s", proc->pid, chars_get, tty_id, proc->tty_buf);
        if(chars_get) {
            proc->exit_code = chars_get; 
            tty_read_dequeue(tty_id);
            en_ready_queue(proc);
        } else {
            break;
        }
        proc = peek_tty_read_queue(tty_id);
    }

    log_info("Ends tty receive: current pid = %u", running_proc->pid);
    return;
}

/*
This interrupt is generated by the terminal device controller hard- ware, when the current
buffer of data previously given to the controller on a TtyTransmit instruction has been completely sent to the terminal.
*/
void trap_tty_transmit_handler(UserContext *user_context)
{
        unsigned int tty_id = user_context->code;
        
        log_info("Starts tty transmit: current pid = %u, tty num = %u", tty_writing_procs[tty_id]->pid, tty_id);
        tty_write_next_ready(tty_id);
        en_ready_queue(tty_writing_procs[tty_id]);
        set_write_proc(NULL, tty_id);
        log_info("Ends tty transmit: current pid = %u", running_proc->pid);
        return;
}


void trap_disk_handler(UserContext *user_context){
	//INVOKE the TRAP_DISK_HANDLER
}

