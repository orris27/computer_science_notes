.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=main
OBJS1=main.o threadpool.o condition.o
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
main:$(OBJS1)
	$(CC) $(CFLAGS) $^ -o $@ -pthread
clean:
	rm -f *.o $(BIN)
