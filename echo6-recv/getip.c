#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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

int getlocalip(char *ip) 
{
    struct hostent *hp;
    char host[256];
    if(gethostname(host, sizeof(host)) < 0) return -1;
    if((hp = gethostbyname(host)) == NULL) return -1;
    strcpy(ip, inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]));

    return 0;
}

int main(int argc, char const *argv[])
{
    char host[256];
    if(gethostname(host, sizeof(host)) < 0) ERR_EIXT("gethostname");//int gethostname(char *name, size_t len)
    // struct hostent *gethostbyname(const char *name)
    struct hostent *hp;
    if((hp = gethostbyname(host)) == NULL) ERR_EIXT("gethostbyname");
    // struct hostent *gethostbyaddr(const void *addr,
    //                                  socklen_t len, int type)

    int i = 0;
    while(hp->h_addr_list[i] != NULL) {
        printf("%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
        ++i;
    }
    char ip[16] = {0};
    if(getlocalip(ip) < 0) ERR_EIXT("getlocalip");
    printf("localip = %s\n", ip);

    return 0;
}