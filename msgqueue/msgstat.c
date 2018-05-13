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
	struct msqid_ds buf;
	msgid = msgget(1234, 0666 | IPC_CREAT);
	// msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT | IPC_EXCL);
	if(msgid == -1) ERR_EIXT("msgget");

	printf("msgget success\n");
	printf("msgid = %d\n", msgid);

	msgctl(msgid, IPC_STAT, &buf);
	printf("buf.msg_perm.mod = %o\n", buf.msg_perm.mode);
	printf("bytes = %ld\n", buf.__msg_cbytes);
	printf("number = %d\n", (int)buf.msg_qnum);
	printf("msgmnb = %d\n", (int)buf.msg_qbytes);

	sscanf("600", "%o", (unsigned int *)&buf.msg_perm.mode);
	msgctl(msgid, IPC_SET, &buf);

	return 0;
}
