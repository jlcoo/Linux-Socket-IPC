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
	int listenfd;
	if((listenfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
		ERR_EIXT("socket");

	unlink("/tmp/test_sockect");
	struct sockaddr_un servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, "/tmp/test_sockect");

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EIXT("bind");
	if(listen(listenfd, SOMAXCONN) < 0)
		ERR_EIXT("listen");

	int conn;
	pid_t pid;
	while(1) {
		conn = accept(listenfd, NULL, NULL);
		if(conn == -1) {
			if(conn == EINTR) continue;
			ERR_EIXT("accept");
		}

		pid = fork();
		if(pid == -1) ERR_EIXT("fork");
		if(pid == 0) {
			close(listenfd);
			echo_srv(conn);
			exit(EXIT_SUCCESS);
		}
		close(conn);
	}

	return 0;
}
