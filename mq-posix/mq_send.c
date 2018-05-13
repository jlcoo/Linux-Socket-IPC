#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

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

int main(int argc, char const *argv[])
{
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <prio>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd_t mqid;
	mqid = mq_open("/abc", O_WRONLY);
	if(mqid == (mqd_t)-1) 
		ERR_EXIT("mq_open");

	STU stu;
	strcpy(stu.name, "test");
	stu.age = 20;

	unsigned int prio = atoi(argv[1]);
	if(mq_send(mqid, (const char*)&stu, sizeof(stu), prio) < 0)
		ERR_EXIT("mq_send") ;
	mq_close(mqid);

	return 0;
}

