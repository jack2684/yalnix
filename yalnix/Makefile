#
#	Sample Makefile for Yalnix kernel and user programs.
#	
#	Prepared by Sean Smith and Adam Salem and various Yalnix developers
#	of years past...
#
#	You must modify the KERNEL_SRCS and KERNEL_OBJS definition below to be your own
#	list of .c and .o files that should be linked to build your Yalnix kernel.
#
#	You must modify the USER_SRCS and USER_OBJS definition below to be your own
#	list of .c and .o files that should be linked to build your Yalnix user programs
#
#	The Yalnix kernel built will be named "yalnix".  *ALL* kernel
#	Makefiles for this lab must have a "yalnix" rule in them, and
#	must produce a kernel executable named "yalnix" -- we will run
#	your Makefile and will grade the resulting executable
#	named "yalnix".
#

#make all will make all the kernel objects and user objects
ALL = $(KERNEL_ALL) $(USER_APPS)
KERNEL_ALL = yalnix

MY_ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
MY_INC = $(MY_ROOT_DIR)/include/
MY_SRC = $(MY_ROOT_DIR)/src/

#List all kernel source files here.  
KERNEL_SRCS = $(MY_SRC)kernel_init.c $(MY_SRC)memory.c $(MY_SRC)list.c $(MY_SRC)dlist.c $(MY_SRC)traps.c $(MY_SRC)proc.c $(MY_SRC)load.c $(MY_SRC)tty.c \
				$(MY_SRC)sys.c $(MY_SRC)timer.c $(MY_SRC)pipe.c $(MY_SRC)aux.c $(MY_SRC)inthashmap.c $(MY_SRC)lock.c $(MY_SRC)sem.c
#List the objects to be formed form the kernel source files here.  Should be the same as the prvious list, replacing ".c" with ".o"
KERNEL_OBJS = $(MY_SRC)kernel_init.o $(MY_SRC)memory.o $(MY_SRC)list.o $(MY_SRC)dlist.o $(MY_SRC)traps.o $(MY_SRC)proc.o $(MY_SRC)load.o $(MY_SRC)tty.o \
				$(MY_SRC)sys.o $(MY_SRC)timer.o $(MY_SRC)pipe.o $(MY_SRC)aux.o $(MY_SRC)inthashmap.o $(MY_SRC)lock.o $(MY_SRC)sem.o
#List all of the header files necessary for your kernel
KERNEL_INCS = 


#List all user programs here.
USER_APPS = $(MY_SRC)init $(MY_SRC)goexec $(MY_SRC)gjj $(MY_SRC)test.tty $(MY_SRC)test.pipe $(MY_SRC)test.lock $(MY_SRC)test.cvar \
			$(MY_SRC)cs58 $(MY_SRC)shell $(MY_SRC)console $(MY_SRC)test.bigstack $(MY_SRC)test.torture $(MY_SRC)test.proc $(MY_SRC)test.sem \
			$(MY_SRC)test.shareframefork $(MY_SRC)test.zero
#List all user program source files here.  SHould be the same as the previous list, with ".c" added to each file
USER_SRCS = $(MY_SRC)init.c $(MY_SRC)goexec.c $(MY_SRC)gjj.c $(MY_SRC)test.tty.c $(MY_SRC)test.pipe.c $(MY_SRC)test.lock.c $(MY_SRC)test.cvar.c \
			$(MY_SRC)cs58.c $(MY_SRC)shell.c $(MY_SRC)console.c $(MY_SRC)test.bigstack.c $(MY_SRC)test.torture.c $(MY_SRC)test.sem.c $(MY_SRC)test.proc.c \
			$(MY_SRC)test.shareframefork.c $(MY_SRC)test.zero.c
#List the objects to be formed form the user  source files here.  Should be the same as the prvious list, replacing ".c" with ".o"
USER_OBJS = $(MY_SRC)init.o $(MY_SRC)goexec.o $(MY_SRC)gjj.o $(MY_SRC)test.tty.o $(MY_SRC)test.pipe.o $(MY_SRC)test.lock.o $(MY_SRC)test.cvar.o \
			$(MY_SRC)cs58.o $(MY_SRC)shell.o $(MY_SRC)console.o $(MY_SRC)test.bigstack.o $(MY_SRC)test.torture.o $(MY_SRC)test.sem.o $(MY_SRC)test.proc.o \
			$(MY_SRC)test.shareframefork.o $(MY_SRC)test.zero.o
#List all of the header files necessary for your user programs
USER_INCS = $(MY_SRC)../include/hardware.h 

#write to output program yalnix
YALNIX_OUTPUT = yalnix

#
#	These definitions affect how your kernel is compiled and linked.
#       The kernel requires -DLINUX, to 
#	to add something like -g here, that's OK.
#

#Set additional parameters.  Students generally should not have to change this next section

#Use the gcc compiler for compiling and linking
CC = gcc

DDIR58 = /net/class/cs58/yalnix
LIBDIR = $(DDIR58)/lib
INCDIR = $(DDIR58)/include
ETCDIR = $(DDIR58)/etc

# any extra loading flags...
LD_EXTRA = 

KERNEL_LIBS = $(LIBDIR)/libkernel.a $(LIBDIR)/libhardware.so

# the "kernel.x" argument tells the loader to use the memory layout
# in the kernel.x file..
KERNEL_LDFLAGS = $(LD_EXTRA) -L$(LIBDIR) -lkernel -lelf -Wl,-T,$(ETCDIR)/kernel.x -Wl,-R$(LIBDIR) -lhardware
LINK_KERNEL = $(LINK.c)

#
#	These definitions affect how your Yalnix user programs are
#	compiled and linked.  Use these flags *only* when linking a
#	Yalnix user program.
#

USER_LIBS = $(LIBDIR)/libuser.a
ASFLAGS = -D__ASM__
CPPFLAGS= -m32 -fno-builtin -I. -I$(INCDIR) -I$(MY_INC) -g -DLINUX 


##########################
#Targets for different makes
# all: make all changed components (default)
# clean: remove all output (.o files, temp files, LOG files, TRACE, and yalnix)
# count: count and give info on source files
# list: list all c files and header files in current directory
# kill: close tty windows.  Useful if program crashes without closing tty windows.
# $(KERNEL_ALL): compile and link kernel files
# $(USER_ALL): compile and link user files
# %.o: %.c: rules for setting up dependencies.  Don't use this directly
# %: %.o: rules for setting up dependencies.  Don't use this directly

all: $(ALL)	

clean:
	rm -f src/*.o src/*~ include/*~ *~ TTYLOG* TRACE $(YALNIX_OUTPUT) $(USER_APPS)  core.*

count:
	wc $(KERNEL_SRCS) $(USER_SRCS)

list:
	ls -l *.c *.h

kill:
	killall yalnixtty yalnixnet yalnix

no-core:
	rm -f core.*

$(KERNEL_ALL): $(KERNEL_OBJS) $(KERNEL_LIBS) $(KERNEL_INCS)
	$(LINK_KERNEL) -o $@ $(KERNEL_OBJS) $(KERNEL_LDFLAGS)

$(USER_APPS): $(USER_OBJS) $(USER_INCS)
	$(ETCDIR)/yuserbuild.sh $@ $(DDIR58) $@.o










