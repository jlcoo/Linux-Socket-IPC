#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

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
	char name[4];
	int age;
} STU;

int main(int argc, char const *argv[])
{
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);
	if(fd == -1)
		ERR_EIXT("open");
	lseek(fd, sizeof(STU)*5-1, SEEK_SET);
	write(fd, "", 1);

	STU *p;
//  void *mmap(void *addr, size_t length, int prot, int flags,
//                  int fd, off_t offset);
//  int munmap(void *addr, size_t length);
	p = (STU*)mmap(NULL, sizeof(STU)*5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(p == NULL) ERR_EIXT("mmap");

	char ch = 'a';
	for(int i = 0; i < 5; ++i) {
		memcpy((p+i)->name, &ch, 1);
		(p+i)->age = 20+i;
		++ch;
	}
	printf("initialize over\n");
	munmap(p, sizeof(STU)*5);
	printf("exit...\n");


	return 0;
}

