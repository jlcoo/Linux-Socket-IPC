#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>

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
    struct rlimit rl;

    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) 
        ERR_EIXT("getrlimit");
    printf("rlim_max = %d, rlim_cur = %d\n", (int)rl.rlim_max, (int)rl.rlim_cur);

    rl.rlim_max = 2048;
    rl.rlim_cur = 2048;
    if(setrlimit(RLIMIT_NOFILE, &rl) < 0) 
        ERR_EIXT("setrlimit");
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) 
        ERR_EIXT("getrlimit");
    printf("rlim_max = %d, rlim_cur = %d\n", (int)rl.rlim_max, (int)rl.rlim_cur);

    return 0;
}