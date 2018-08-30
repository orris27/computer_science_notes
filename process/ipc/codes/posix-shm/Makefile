.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=open delete stat send recv
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
open:open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
delete:delete.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
stat:stat.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
send:send.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
recv:recv.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
clean:
	rm -f *.o $(BIN)
