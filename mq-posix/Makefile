.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = mq_open mq_ulink mq_getaddr mq_send mq_recv mq_notify
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mq_open: mq_open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_ulink: mq_ulink.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_getaddr: mq_getaddr.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_send: mq_send.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_recv: mq_recv.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_notify: mq_notify.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm -f *.o $(BIN)