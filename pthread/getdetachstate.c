#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/mman.h>
#include <mqueue.h>
#include <pthread.h>

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

int main(int argc, char const *argv[])
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	int state;
	pthread_attr_getdetachstate(&attr, &state);
	if(state == PTHREAD_CREATE_JOINABLE)
		printf("detachstate: PTHREAD_CREATE_JOINABLE\n");
	else if(state == PTHREAD_CREATE_DETACHED)
		printf("detachstate: PTHREAD_CREATE_DETACHED\n");

	size_t size;
	pthread_attr_getstacksize(&attr, &size);
	printf("stacksize:%lu\n", size);

	pthread_attr_getguardsize(&attr, &size);
	printf("guardsize:%lu\n", size);

	int scope;
	pthread_attr_getscope(&attr, &scope);
	if(scope == PTHREAD_SCOPE_SYSTEM)
		printf("scope: PTHREAD_SCOPE_SYSTEM\n");
	else if(scope == PTHREAD_SCOPE_PROCESS)
		printf("scope: PTHREAD_SCOPE_PROCESS\n");

	int policy;
	pthread_attr_getschedpolicy(&attr, &policy);
	if(policy == SCHED_FIFO)
		printf("policy: SCHED_FIFO\n");
	else if(policy == SCHED_RR)
		printf("policy: SCHED_RR\n");
	else if(policy == SCHED_OTHER)
		printf("policy: SCHED_OTHER\n");

	int inheritsched;
	pthread_attr_getinheritsched(&attr, &inheritsched);
	if(inheritsched == PTHREAD_INHERIT_SCHED)
		printf("inheritsched: PTHREAD_INHERIT_SCHED\n");
	else if(inheritsched == PTHREAD_EXPLICIT_SCHED)
		printf("inheritsched: PTHREAD_EXPLICIT_SCHED\n");

	struct sched_param param;
	pthread_attr_getschedparam(&attr, &param);
	printf("param.sched_priority: %d\n", param.sched_priority);

	int level;
	level = pthread_getconcurrency();
	printf("level: %d\n", level);

	return 0;
}

