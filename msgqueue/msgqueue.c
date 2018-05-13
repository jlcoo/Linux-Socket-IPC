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

int main(int argc, char const *argv[])
{
	int msgid;
	// msgid = msgget(1234, 0666 | IPC_CREAT | IPC_EXCL);
	msgid = msgget(1234, 0666 | IPC_CREAT);
	// msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT | IPC_EXCL);
	if(msgid == -1) ERR_EIXT("msgget");

	printf("msgget success\n");
	printf("msgid = %d\n", msgid);

	msgctl(msgid, IPC_RMID, NULL);

	return 0;
}
