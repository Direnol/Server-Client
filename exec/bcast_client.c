#include "../headers/Broadcast.h"

char ip[] = "192.168.1.8", mask[] = "255.255.255.0";

int main(int argc, char **argv)
{
    broadcast_t sock;
    init_bcast(&sock, BCLIENT, ip, mask, 7777);

    int ep = epoll_create(2);
    size_t size = BUFSIZ;
    struct epoll_event *ev = malloc(2 * sizeof(*ev));
    char *buff = malloc(size);
    if (ev == NULL || buff == NULL) {
        perror("Alloc");
        sock.status = OFF;
    }
    if (sock.status == ON) {
        ev[0].events = ev[1].events = EPOLLIN;
        ev[0].data.fd = STDIN_FILENO;
        ev[1].data.fd = sock.sock.sock.fd;
        for (int i = 0; i < 2; ++i) {
            epoll_ctl(ep, EPOLL_CTL_ADD, ev[i].data.fd, ev + i);
        }
    }
    while (sock.status) {
        int count = epoll_wait(ep, ev, 2, 3000);
        if (count == 0) continue;
        if (count < 0) {
            perror("Epoll wait");
            break;
        }
        for (int i = 0; i < count; ++i) {
            if (ev[i].data.fd == STDIN_FILENO) {
                sock.status = OFF;
                break;
            } else {
                struct sockaddr_in from;
                bcast(sock, (void **) &buff, &size, &from);
                sinfo_t info;
                get_addr(from, &info);
                printf("From[%s:%d]:%s\n", info.ip, info.port, buff);
            }
        }
    }
    free(ev);
    free(buff);
    close(ep);
    close_bcast(sock);
    return EXIT_SUCCESS;
}