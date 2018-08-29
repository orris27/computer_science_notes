.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=msg-create msg-rm msg-stat msg-set msg-snd opt msg-rcv
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
