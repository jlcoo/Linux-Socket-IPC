.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = pthread-create echo-cli echo-srv getdetachstate thread-spicific
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
pthread-create: pthread-create.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
echo-cli: echo-cli.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
echo-srv: echo-srv.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
getdetachstate: getdetachstate.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
thread-spicific: thread-spicific.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
# mq_notify: mq_notify.o
# 	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm -f *.o $(BIN)