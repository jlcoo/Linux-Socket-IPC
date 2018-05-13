#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>

#define MAX_EVENT 1024
#define BACKLOG 128
#define MAX_DATALEN 1024

typedef struct sockaddr SA;

int set_tcp_reuse(int sock)
{
    int opt = 1;
    return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
}

int set_tcp_nodelay(int sock)
{
    int opt = 1;
    return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

int set_non_block(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, NULL);
    if (flags == -1) {
        perror("fcntl F_GETFL error");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL error");
        return -1;
    }
    return 0;
}

int ignore_sigpipe()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL)) {
        perror("sigaction error");
        return -1;
    }
    return 0;
}

int create_tcp_server(const char *ip, uint16_t port, int backlog)
{
    int ret = -1;
    socklen_t len = 0;

    if (ignore_sigpipe()) {
        printf("setting ignore sigpipe failed\n");
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("create socket error");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    if (!ip) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        ret = inet_pton(AF_INET, ip, (SA *)&addr.sin_addr);
        if (ret <= 0) {
            if (ret == 0) {
                fprintf(stderr, "not invalid ip: [%s]\n", ip);
            } else {
                perror("inet_pton error");
            }
            return -1;
        }
    }

    if (set_tcp_reuse(sock) == -1) {
        perror("setsockopt SO_REUSEADDR error");
        return -1;
    }

    if (set_tcp_nodelay(sock) == -1) {
        perror("setsockopt TCP_NODELAY error");
        return -1;
    }

    len = sizeof(SA);
    if (bind(sock, (SA *)&addr, len) == -1) {
        perror("bind error");
        return -1;
    }

    if (listen(sock, backlog) == -1) {
        perror("listen error");
        return -1;
    }

    return sock;
}

int set_read_event(int epfd, int ctl, int fd)
{
    struct epoll_event ev;
    ev.data.u64 = 0;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(epfd, ctl, fd, &ev) == -1) {
        perror("epoll_ctl error");
        return -1;
    }
    return 0;
}

int handle_recv(int fd)
{
    int read = 0;
    int n = 0;
    int ret = 0;
    char buf[MAX_DATALEN + 1] = {0};
    int space = 0;
    size_t cap = MAX_DATALEN;
    while (1) {
        n = recv(fd, buf + read, cap - read, 0);
        if (n < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            } else {
                ret = -1;
                break;
            }
        } else if (n == 0) {
            ret = -1;
            break;
        } else {
            space = cap - read;
            read += n;
            if (n == space) {
                if (cap - read == 0) {
                    break;
                }
                continue;
            } else {
                break;
            }
        }
    }

    buf[read] = '\0';

    if (ret < 0) {
        return -1;
    }

    // not a good way, send can fail.
    send(fd, buf, read, 0);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *ip = NULL;
    uint16_t port = 32667;
    int srv = create_tcp_server(ip, port, BACKLOG);
    if (srv < 0) {
        fprintf(stderr, "create tcp server failed\n");
        return -1;
    }

    struct epoll_event ev, events[MAX_EVENT];
    int epfd = -1;
    if ((epfd = epoll_create(1)) == -1) {
        perror("epoll_create error");
        return -1;
    }
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = srv;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, srv, &ev) == -1) {
        perror("epoll_ctl add srv error");
        close(srv);
        return -1;
    }
    int i, ret, cli, fd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(SA);
    while (1) {
        ret = epoll_wait(epfd, events, MAX_EVENT, -1);
        if (ret > 0) {
            for (i = 0; i < ret; i++) {
                fd = events[i].data.fd;
                if (events[i].events & EPOLLIN) {
                    if (fd == srv) {
                        cli = accept(srv, (SA *)&addr, &addrlen);
                        if (cli == -1) {
                            perror("accept error");
                        } else {
                            if (set_non_block(cli) == -1) {
                                fprintf(stderr, "set cli non block failed\n");
                                close(cli);
                                continue;
                            }
                            if (set_read_event(epfd, EPOLL_CTL_ADD, cli) != 0) {
                                close(cli);
                                continue;
                            }
                        }
                    } else {
                        if (handle_recv(fd) == -1) {
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL)) {
                                perror("epoll_ctl EPOLL_CTL_DEL cli error");
                            }
                            close(fd);
                            continue;
                        }
                    }
                }
            }
        } else if (ret == 0) {
            continue;
        } else {
            perror("epoll_wait error");
            continue;
        }
    }
    return 0;
}