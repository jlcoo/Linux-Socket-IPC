#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define ERR_EIXT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

typedef struct stu {
	int age;
	char name[32];	
} STU;

int main(int argc, char const *argv[])
{
	// int shmget(key_t key, size_t size, int shmflg);
	int shmid;
	shmid = shmget(1234, sizeof(STU), IPC_CREAT | 0666);
	if(shmid == -1) ERR_EIXT("shmget");

	STU *p;
	p = shmat(shmid, NULL, 0);
	if(p == (void*)-1) 
		ERR_EIXT("shmat");
	// sleep(10);
	printf("name = %s age = %d\n", p->name, p->age);
	// sscanf("quit", "%s", p->name);
	memcpy(p, "quit", 4);
	shmdt(p);

	return 0;
}

