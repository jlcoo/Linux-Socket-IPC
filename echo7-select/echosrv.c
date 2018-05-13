#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
struct packet {
    int len;
    char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char*) buf;

    while(nleft > 0) {
        if((nread = read(fd, bufp, nleft)) < 0) {
            if(errno == EINTR)
                continue;
            return -1;
        } else if(nread == 0)
            return count - nleft;
        bufp += nread;
        nleft -= nread;
    }

    return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char*) buf;

    while(nleft > 0) {
        if((nwritten = write(fd, bufp, nleft)) < 0) {
            if(errno == EINTR)
                continue;
            return -1;
        } else if(nwritten == 0)
            return count - nleft;
        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while(1) {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if(ret == -1 && errno == EINTR) continue;

        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline;

    while(1) {
        ret = recv_peek(sockfd, bufp, nleft);
        if(ret < 0) return ret;
        else if(ret == 0) return ret;
        nread = ret;
        int i;
        for(i = 0; i < nread; ++i) {
            if(bufp[i] == '\n') {
                ret = readn(sockfd, bufp, i+1);
                if(ret != i+1)
                    ERR_EIXT("readn failed");
                return ret;
            }
        }
        if(nread > nleft) exit(EXIT_FAILURE);
        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if(ret != nread)
            ERR_EIXT("readn failed");
        bufp += nread;
    }

    return -1;
}

void do_service(int conn) 
{
    // struct packet recvbuf;
    char recvbuf[1024];
    // int n;

    while(1) {
        memset(&recvbuf, 0, sizeof(recvbuf));
        int ret = readline(conn, recvbuf, 1024); //conn是已连接套接字, 是主动套接字
        if(ret == -1) ERR_EIXT("read failed");
        else if(ret == 0) {
            printf("client close\n");
            break;
        } 
        fputs(recvbuf, stdout);
        writen(conn, recvbuf, strlen(recvbuf));
    }
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

    pid_t pid;
    while(1) {
        if((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
            ERR_EIXT("accept failed");
        printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

        pid = fork();
        if(pid == -1) ERR_EIXT("fork failed");
        if(pid == 0) {
            close(listenfd);
            do_service(conn);
            exit(EXIT_SUCCESS);
        } else {
            close(conn);
        }
    }

    return 0;
}