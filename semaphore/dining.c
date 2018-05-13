#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

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

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

int sem_p(int semid)
{
	struct sembuf sb = {0, -1, 0};
	int ret;
	ret = semop(semid, &sb, 1);
	if(ret == -1)
		ERR_EIXT("semopt");

	return ret;
}

int sem_v(int semid)
{
	struct sembuf sb = {0, 1, 0};
	int ret;
	ret = semop(semid, &sb, 1);
	if(ret == -1)
		ERR_EIXT("semopt");

	return ret;
}

#define DELAY ((rand() % 5) + 1)

int semid;

//等待一把叉子
int wait_1fork(int no)
{
    struct sembuf sb = {no, -1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if (ret == -1)
        ERR_EIXT("semop");

    return ret;
}


void wait_for_2fork(int no)
{
	int left = no;
	int right = (no + 1) % 5;
	struct sembuf buf[2] = {
				{left, -1, 0},
				{right, -1, 0}
			};
	semop(semid, buf, 2);
}

void free_2fork(int no)
{
	int left = no;
	int right = (no + 1) % 5;
	struct sembuf buf[2] = {
				{left, 1, 0},
				{right, 1, 0}
			};
	semop(semid, buf, 2);
}

void philosophere(int no) 
{
	srand(getpid());
	for(;;) {
		/*printf("%d is thinking\n", no);
		sleep(DELAY);
		printf("%d is hungry\n", no);
		wait_for_2fork(no);
		printf("%d is eating\n", no);
		sleep(DELAY);
		free_2fork(no);*/

		int left = no;
        int right = (no + 1) % 5;
        printf("%d is thinking\n", no);
        sleep(DELAY);
        printf("%d is hungry\n", no);
        wait_1fork(left);
        sleep(DELAY);
        wait_1fork(right);
        printf("%d is eating\n", no);
        sleep(DELAY);
        free_2fork(no);
	}
}

int main(int argc, char const *argv[])
{
	semid = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
	if(semid == -1) 
		ERR_EIXT("semget");
	union semun su;
	su.val = 1;
	for(int i = 0; i < 5; ++i) {
		semctl(semid, i, SETVAL, su);
	}

	int no = 0;
	pid_t pid;
	for(int i = 0; i < 5; ++i) {
		pid = fork();
		if(pid == -1) 
			ERR_EIXT("fork");
		if(pid == 0) {
			no = i;
			break;
		}
	}
	philosophere(no);

	return 0;
}

