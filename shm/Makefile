.PHONY: clean all
CC = gcc
CFLAGS = -Wall -g
BIN = shm_open shm_unlink shm_write shm_read
all: $(BIN)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
shm_open: shm_open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_unlink: shm_unlink.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_write: shm_write.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_read: shm_read.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
# mq_recv: mq_recv.o
# 	$(CC) $(CFLAGS) $^ -o $@ -lrt
# mq_notify: mq_notify.o
# 	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm -f *.o $(BIN)