.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=wait exit attr key
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
wait:wait.o
	$(CC) $(CFLAGS) $^ -o $@ -pthread
exit:exit.o
	$(CC) $(CFLAGS) $^ -o $@ -pthread
attr:attr.o
	$(CC) $(CFLAGS) $^ -o $@ -pthread
key:key.o
	$(CC) $(CFLAGS) $^ -o $@ -pthread
clean:
	rm -f *.o $(BIN)
