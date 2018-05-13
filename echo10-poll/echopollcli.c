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

int main(int argc, char const *argv[])
{
    int count = 0;
    while(1) {
        int sock;
        if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
            sleep(4);
            ERR_EIXT("socket failed");
        }
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

        struct sockaddr_in localaddr;
        socklen_t addrlen = sizeof(localaddr);
        if(getsockname(sock,(struct sockaddr*)&localaddr, &addrlen) < 0)
            ERR_EIXT("getsockname");
        printf("ip=%s port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
        printf("count = %d\n", ++count);
    }


    return 0;
}