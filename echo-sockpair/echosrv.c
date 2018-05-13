#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/un.h>

#define ERR_EIXT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)
void echo_srv(int conn)
{
	char recvbuf[1024] = {0};
	int n;

	while(1) {
		memset(recvbuf, 0, sizeof(recvbuf));
		n = read(conn, recvbuf, sizeof(recvbuf));
		if(n == -1) {
			if(errno == EINTR) continue;
			ERR_EIXT("read");
		} else if(n == 0) {
			printf("client close\n");
			break;
		} else if(n > 0) {
			fputs(recvbuf, stdout);
			write(conn, recvbuf, n);
		}
	}
	// close()
	close(conn);
}

int main(int argc, char const *argv[])
{
	int sockfds[2];

	if(socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds) < 0)
		ERR_EIXT("socketpair");

	pid_t pid;
	pid = fork();
	if(pid == -1) ERR_EIXT("fork");
	if(pid == 0) {
		int val;
		close(sockfds[0]);
		while(1) {
			read(sockfds[1], &val, sizeof(val));
			++val;
			write(sockfds[1], &val,sizeof(val));
		}
	} else if(pid > 0) {
		int val = 0;
		close(sockfds[1]);
		while(1) {
			++val;
			printf("sending data: %d\n", val);
			write(sockfds[0], &val, sizeof(val));
			read(sockfds[0], &val, sizeof(val));
			printf("data received: %d\n", val);
			sleep(1);
		}
	}	

	return 0;
}
