.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = ptest pthread-cond main
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
ptest: ptest.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
pthread-cond: pthread-cond.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
main: main.o threadpool.o condition.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread