#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define ERR_EXIT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)


int main(int argc, char const *argv[])
{
	mqd_t mqid;
	mqid = mq_open("/abc", O_RDONLY);
	if(mqid == (mqd_t)-1) 
		ERR_EXIT("mq_open");

	printf("mq_open success\n");
	struct mq_attr attr;
	mq_getattr(mqid, &attr);
	printf("max #mag=%ld max #bytes/msg=%ld #currently on queue=%ld\n", 
		attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

	return 0;
}

