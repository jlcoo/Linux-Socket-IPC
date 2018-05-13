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

void echo_cli(int msgid)
{
	int pid = getpid();
	int n;
	struct msgbuf msg;
	memset(&msg, 0, sizeof(msg));

	msg.mtype = 1;
	*((int *)msg.mtext) = pid;
	while(fgets(msg.mtext+4, MSGMAX-4, stdin) != NULL) {
		msg.mtype = 1;
		if(msgsnd(msgid, &msg, strlen(msg.mtext+4)+4, 0) < 0)
			ERR_EIXT("msgsnd");

		memset(msg.mtext+4, 0, MSGMAX-4);
		if((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0)
			ERR_EIXT("msgsnd");
		fputs(msg.mtext+4, stdout);
		memset(msg.mtext+4, 0, MSGMAX-4);
	}
}

int main(int argc, char const *argv[])
{
	int msgid;
	msgid = msgget(1234, 0);
	if(msgid == -1) ERR_EIXT("msgget");

	echo_cli(msgid);

	return 0;
}
