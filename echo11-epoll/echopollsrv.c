#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>

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

void handle_sigpipe(int sig) 
{
    printf("recv a sig = %d\n", sig);
}

int main(int argc, char const *argv[])
{
    // signal(SIGPIPE, SIG_IGN);
    signal(SIGPIPE, handle_sigpipe);
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
    socklen_t peerlen;
    int conn;

    struct pollfd client[2048];
    int i;
    int maxi = 0; //zui da bu kong xian wei zi
    for(i = 0; i < 2048; ++i) 
        client[i].fd = -1;
    // int maxfd = listenfd;
    int nready;
    int count = 0;
    client[0].fd = listenfd;
    client[0].events = POLLIN;
    while(1) {
        nready = poll(client, maxi+1, -1);
        // nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(nready == -1) {
            if(errno == EINTR) //xin hao zhong duan
                continue;
            ERR_EIXT("select");
        } else if(nready == 0) continue;
        if(client[0].revents & POLLIN) {
            peerlen = sizeof(peeraddr);  //peerlen一定要有初始值
            if((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
                ERR_EIXT("accept failed");
            for(i = 0; i < 2048; ++i) {
                if(client[i].fd < 0) {
                    client[i].fd = conn;
                    if(i > maxi)
                        maxi = i;
                    break;
                }
            }
            if(i == 2048) {
                fprintf(stderr, "too many clients\n");
                exit(EXIT_FAILURE);
            }
            printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
            printf("count = %d\n", ++count);

            client[i].events = POLLIN;

            if(--nready <= 0) continue;
        }

        for(i = 1; i <= maxi; ++i) {
            conn = client[i].fd;
            if(conn == -1) continue;
            if(client[i].events & POLLIN) {
                char recvbuf[1024];
                memset(&recvbuf, 0, sizeof(recvbuf));
                int ret = readline(conn, recvbuf, 1024); //conn是已连接套接字, 是主动套接字
                if(ret == -1) ERR_EIXT("readline");
                else if(ret == 0) {
                    printf("client close\n");
                    client[i].fd = -1;
                    close(conn);
                } 
                fputs(recvbuf, stdout);
                // sleep(3);
                writen(conn, recvbuf, strlen(recvbuf));

                if(--nready <= 0) break;
            }
        }

    }

    return 0;
}