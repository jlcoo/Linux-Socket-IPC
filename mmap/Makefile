.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = mmap mmap-read shmwrite shmread
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)