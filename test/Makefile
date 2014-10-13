.SUFFIXES: .c

SRCS = list.test.c ../include/standard/list.c
OBJS = $(SRCS:.c=.o)
OUTPUT = list.test

CC = gcc 
CFLAGS = -g -m32
LIBS = 

test: $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(OUTPUT)

depend:
	makedepend -I/usr/local/include/g++ -- $(CFLAGS) -- $(SRCS) 

#DO NOT DELETE
#
#
#
