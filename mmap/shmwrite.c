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
	strcpy(p->name, "lisi");
	p->age = 20;
	// sleep(10);
	// printf("name = %s age = %d\n", p->name, p->age);
	while(1) {
		if(memcmp(p, "quit", 4) == 0) break;
	}
	shmdt(p);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}

