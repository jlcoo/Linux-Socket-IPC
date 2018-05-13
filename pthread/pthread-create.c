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

void* thread_routine(void *arg)
{
	for(int i = 0; i < 20; ++i) {
		printf("B");
		fflush(stdout);
		usleep(20);
		// if(i == 3)
		// 	pthread_exit("abc");
	}
	sleep(2);

	return "def";
}

int main(int argc, char const *argv[])
{
	pthread_t tid;
	int ret;
	if((ret = pthread_create(&tid, NULL, thread_routine, NULL)) != 0) {
		fprintf(stderr, "pthread_create: %s\n", strerror(ret));
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < 20; ++i) {
		printf("A");
		fflush(stdout);
		usleep(20);
	}

	// int pthread_join(pthread_t thread, void **retval); //wait_pid
	void *value;
	if((ret = pthread_join(tid, &value)) != 0) {
		fprintf(stderr, "pthread_join: %s\n", strerror(ret));
		exit(EXIT_FAILURE);
	}

	// pthread_detach tuo li jiang xian cheng

	// void pthread_exit(void *retval);
	printf("\n");
	printf("return msg = %s\n", (char*)value);

	return 0;
}

