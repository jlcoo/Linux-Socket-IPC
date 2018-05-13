#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <signal.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define ERR_EXIT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

typedef struct stu {
	char name[32];
	int age;
} STU;

size_t size;
mqd_t mqid;
struct sigevent sev;

void handle_sigusr1(int sig) {
	mq_notify(mqid, &sev);
	
	STU stu;
	unsigned int prio;
	if(mq_receive(mqid, (char*)&stu, size, &prio) < 0)
		ERR_EXIT("mq_send") ;

	printf("name = %s age = %d prio = %u\n", stu.name, stu.age, prio);
}

int main(int argc, char const *argv[])
{

	mqid = mq_open("/abc", O_RDONLY);
	if(mqid == (mqd_t)-1) 
		ERR_EXIT("mq_open");

	signal(SIGUSR1, handle_sigusr1);

	struct mq_attr attr;
	mq_getattr(mqid, &attr);
	size = attr.mq_msgsize;

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;

	mq_notify(mqid, &sev);
	for(;;)
		pause();
	
	mq_close(mqid);

	return 0;
}

