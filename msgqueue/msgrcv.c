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

#define MSGMAX 8192

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main(int argc, char const *argv[])
{
	int flag = 0;
	int type = 0;
	int opt;

	while(1) {
		opt = getopt(argc, (char *const *)argv, "nt:");
		if(opt == '?')
			exit(EXIT_FAILURE);
		if(opt == -1) break;

		switch(opt) {
			case 'n':
				// printf("AAAAA\n");
				flag |= IPC_NOWAIT;
				break;
			case 't':
				// printf("BBBBBB\n");
				// int n = atoi(optarg);
				// printf("n=%d\n", n);
				type = atoi(optarg);
				break;
		}
	}

	// int len = atoi(argv[1]);
	int msgid;

	msgid = msgget(1234, 0666 | IPC_CREAT);
	if(msgid == -1) ERR_EIXT("msgget");

	struct msgbuf *ptr;
	ptr = (struct msgbuf*)malloc(sizeof(long) + MSGMAX);
	int n = 0;
	if((n = msgrcv(msgid, ptr, MSGMAX, type, flag)) < 0)
		ERR_EIXT("msgsnd");
	printf("read %d bytes type = %ld\n", n, ptr->mtype);


	return 0;
}
