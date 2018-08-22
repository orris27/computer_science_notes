.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=endian
all:$(BIN)
$.o:%.c
    $(CC) $(CFLAGS) -c $< -o $@
clean:
    rm -f *.o $(BIN)
orris@orris-Laptop:~/fun/network/soc
