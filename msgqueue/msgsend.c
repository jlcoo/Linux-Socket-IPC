#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define ERR_EIXT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main(int argc, char const *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <bytes> <type>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int len = atoi(argv[1]);
	int type = atoi(argv[2]);
	int msgid;
	// msgid = msgget(1234, 0666 | IPC_CREAT | IPC_EXCL);
	// struct msqid_ds buf;
	msgid = msgget(1234, 0666 | IPC_CREAT);
	// msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT | IPC_EXCL);
	if(msgid == -1) ERR_EIXT("msgget");

	struct msgbuf *ptr;
	ptr = (struct msgbuf*)malloc(sizeof(long) + len);
	ptr->mtype = type;
	if(msgsnd(msgid, ptr, len, IPC_NOWAIT) < 0)
		ERR_EIXT("msgsnd");

	return 0;
}
