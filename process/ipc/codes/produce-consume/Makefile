.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=produce consume
OBJS1=produce.o fifo.o sem.o
OBJS2=consume.o fifo.o sem.o
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
produce:$(OBJS1)
	$(CC) $(CFLAGS) $^ -o $@
consume:$(OBJS2)
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f *.o $(BIN)
