.SUFFIXES: .c

SRCS = list.test.c ../src/list.c  
OBJS = $(SRCS:.c=.o)
OUTPUT = list.test

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
DDIR58 = $(ROOT_DIR)
LIBDIR = $(DDIR58)/../lib
INCDIR = $(DDIR58)/../include

CC = gcc 
CFLAGS = -g -m32
LIBS = 

test: $(OBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $(OUTPUT) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(OUTPUT)

depend:
	makedepend -I/usr/local/include/g++ -- $(CFLAGS) -- $(SRCS) 

#DO NOT DELETE
#
#
#
