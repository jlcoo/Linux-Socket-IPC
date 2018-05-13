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
    char mtext[MSGMAX];    /* message data */
};

void echo_srv(int msgid)
{
	int n = 0;
	struct msgbuf msg;
	memset(&msg, 0, sizeof(msg));

	while(1) {
		if((n = msgrcv(msgid, &msg, MSGMAX, 1, 0)) < 0)
			ERR_EIXT("msgsnd");
		int pid = *((int *)msg.mtext);

		fputs(msg.mtext + 4, stdout);

		msg.mtype = pid;
		printf("msg.mtype = %ld\n", msg.mtype);
		msgsnd(msgid, &msg, n, 0);
		memset(&msg, 0, sizeof(msg));
	}
}

int main(int argc, char const *argv[])
{
	int msgid;
	msgid = msgget(1234, 0666 | IPC_CREAT);
	if(msgid == -1) ERR_EIXT("msgget");

	echo_srv(msgid);

	return 0;
}
