#include "shmfifo.h"

typedef struct stu {
	char name[32];
	int age;
} STU;

int main(int argc, char const *argv[])
{
	shmfifo_t *fifo = shmfifo_init(1234, sizeof(STU), 3);
	shmfifo_destroy(fifo);

	return 0;
}

