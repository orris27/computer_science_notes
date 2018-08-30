.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mq-create mq-delete mq-stat mq-send mq-recv mq-notify
all:$(BIN)
$.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
mq-create:mq-create.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq-delete:mq-delete.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq-stat:mq-stat.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq-send:mq-send.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq-recv:mq-recv.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq-notify:mq-notify.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
clean:
	rm -f *.o $(BIN)
