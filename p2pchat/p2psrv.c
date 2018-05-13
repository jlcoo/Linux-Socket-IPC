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
    int listenfd;
    if((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
        ERR_EIXT("socket failed");
    // listenfd = socket(PF_INET, SOCK_STREAM, 0);
    struct  sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //INADDR_ANY表示本机的任意地址, 网络字节序
    // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //显示指定本机的IP地址
    // inet_aton("127.0.0.1", &servaddr.sin_addr)
    int on = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)  //允许关闭后马上重启
        ERR_EIXT("setsockopt failed");

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EIXT("bind failed");
    if(listen(listenfd, SOMAXCONN) < 0)
        ERR_EIXT("listen failed");  // SOMAXCONN是队列允许的最大值

    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);  //peerlen一定要有初始值
    int conn;
    if((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
        ERR_EIXT("accept failed");

    printf("ip=%s port=%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

    pid_t pid;
    pid = fork();
    if(pid == -1) ERR_EIXT("fork failed");
    if(pid == 0) {
    	signal(SIGUSR1, handler);
        char sendbuf[1024];
        while(fgets(sendbuf,sizeof(sendbuf), stdin) != NULL) {
            write(conn, sendbuf, strlen(sendbuf)); 
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        printf("child close\n");
        exit(EXIT_SUCCESS);
    }else {
        char recvbuf[1024];
        while(1) {
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(conn, recvbuf, sizeof(recvbuf)); //conn是已连接套接字, 是主动套接字
            if(ret == -1) 
                ERR_EIXT("read failed");
            else if(ret == 0) {
                printf("peer close\n");
                break;
            }
            else {
                fputs(recvbuf, stdout);
            }
        }
        printf("parent close\n");
        kill(pid, SIGUSR1);
        close(conn);
        close(listenfd);
        exit(EXIT_SUCCESS);
    }

    return 0;
}