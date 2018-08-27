.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=client server icmp-error-client
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
