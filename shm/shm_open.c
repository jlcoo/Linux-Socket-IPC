#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/mman.h>
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
	int shmid;
	shmid = shm_open("/xyz", O_CREAT | O_RDWR, 0666);
	if(shmid == -1) 
		ERR_EXIT("shm_open");
	printf("shm_open success\n");

	if(ftruncate(shmid, sizeof(STU)) == -1)
		ERR_EXIT("ftruncate");

	struct stat buf;
	if(fstat(shmid, &buf) == -1)
		ERR_EXIT("fstat");
	printf("size=%ld mode=%o\n", buf.st_size, buf.st_mode & 07777);

	close(shmid);

	return 0;
}

