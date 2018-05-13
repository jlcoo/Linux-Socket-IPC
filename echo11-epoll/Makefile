.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = echosrv echocli echopollsrv echopollcli
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

epollsrv:epollsrv.o
	g++ $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(BIN)