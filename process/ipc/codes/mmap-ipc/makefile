.PHONY:clean all
src=$(wildcard ./*.c)
objs=$(patsubst %.c,%,$(src))
target=main
CC=gcc
CFLAGS=-Wall -g
#CPPFLAGS=-I
all:$(objs)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(objs) $(target)
