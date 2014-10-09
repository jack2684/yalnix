//These are the traps used in Yalnix
//These introduction for each TRAP is from the Yalnix 2014.pdf

//This trap results from a “kernel call” trap instruction executed by the current user processes. 
//Such a trap is used by user processes to request some type of service from the operating system kernel, 
//such as creating a new process, allocating memory, or performing I/O. 
//All different kernel call requests enter the kernel through this single type of trap
void TRAP_KERNEL(signal){
	//SWITCH(signal){
		//EXECUTE different system calls
	//END SWITCH
}


/*This exception results from any arithmetic error from an 
instruction executed by the current user process, such as division by zero or an arithmetic overflow.*/
void TRAP_MATH(signal){
	//KILL the process with the arithmetic error
}

/*This exception results from the execution of an illegal instruction by the currently executing user process. 
An illegal instruction can be an undefined machine lan- guage opcode, an illegal addressing mode, 
or a privileged instruction when not in kernel mode.*/
void TRAP_ILLEGAL(signal){
	//KILL the process executing the illegal instruction
}

/*
This exception results from a disallowed memory access by the current user process. 
The access may be disallowed because the address is outside the virtual address range of the hardware (outside Region 0 and Region 1), 
because the address is not mapped in the current page tables, 
or because the access violates the page protection specified in the corresponding page table entry.
*/
void TRAP_MEMORY(signal){
	//KILL the process that cause the the disallowed memory access
}

//This interrupt results from the machine’s hardware clock, which generates periodic clock interrupts
void TRAP_CLOCK(signal){
	//INVOKE the TRAP_CLOCK handler
}

/*
This interrupt is generated by the terminal device controller hard- ware, 
when a complete line of input is available from one of the terminals attached to the system.
*/
void TRAP_TTY_RECEIVE(signal){
	//INVOKE the TRAP_TTY_RECEIVE handler
}

/*
This interrupt is generated by the terminal device controller hard- ware, when the current
buffer of data previously given to the controller on a TtyTransmit instruction has been completely sent to the terminal.
*/
void TRAP_TTY_TRANSMIT(signal){
	//INVOKE the TRAP_TTY_TRANSMIT handler
}

void TRAP_DISK(signal){
	//INVOKE the TRAP_DISK handler
}

