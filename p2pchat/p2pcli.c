#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EIXT(m) \
        do \
        {  \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)
void handler(int sig)
{
    printf("recv a sig = %d\n", sig);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    int sock;
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
        ERR_EIXT("socket failed");
    // listenfd = socket(PF_INET, SOCK_STREAM, 0);
    struct  sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //INADDR_ANY表示本机的任意地址, 网络字节序
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //显示指定本机的IP地址
    // inet_aton("127.0.0.1", &servaddr.sin_addr)
    if(connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EIXT("connect failed");

    pid_t pid;
    pid = fork();
    if(pid == -1) ERR_EIXT("fork failed");
    if(pid == 0) {
        char recvbuf[1024] = {0};
        while(1) {
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(sock, recvbuf, sizeof(recvbuf));
            if(ret == -1) ERR_EIXT("read failed");
            else if(ret == 0) {
                printf("peer close\n");
                break;
            }
            else fputs(recvbuf, stdout);
        }
        close(sock);
        printf("child close\n");
        kill(pid, SIGUSR1);
        exit(EXIT_SUCCESS);
    } else {
        signal(SIGUSR1, handler);
        char sendbuf[1024] = {0}; 
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
            write(sock, sendbuf, strlen(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        close(sock);
        printf("parent close\n");
        exit(EXIT_SUCCESS);
    }
    // close(sock);

    return 0;
}