.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=server client multiclient fin_wait_client
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
