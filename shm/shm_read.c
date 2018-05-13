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
	shmid = shm_open("/xyz", O_RDONLY, 0);
	if(shmid == -1) 
		ERR_EXIT("shm_open");
	printf("shm_open success\n");

	struct stat buf;
	if(fstat(shmid, &buf) == -1)
		ERR_EXIT("fstat");

	// printf("size=%ld mode=%o\n", buf.st_size, buf.st_mode & 07777);
	STU *p;
	p = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, shmid, 0);
	if(p == MAP_FAILED)
		ERR_EXIT("mmap");

	printf("name = %s age = %d\n", p->name, p->age);

	close(shmid);

	return 0;
}

