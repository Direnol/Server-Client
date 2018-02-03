#include "../headers/Broadcast.h"

char ip[] = "192.168.1.8", mask[] = "255.255.255.0";

int main(int argc, char **argv)
{
    broadcast_t sock;
    init_bcast(&sock, BSERV, ip, mask, 7777);

    int ep = epoll_create(2);
    size_t size = BUFSIZ;
    struct epoll_event ev;
    char *buff = malloc(size);
    if (buff == NULL) {
        perror("Alloc");
        sock.status = OFF;
    }
    if (sock.status == ON) {
        ev.events = EPOLLIN;
        ev.data.fd = STDIN_FILENO;
        epoll_ctl(ep, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    }

    while (sock.status) {
        int count = epoll_wait(ep, &ev, 1, 5000);
        if (count == 0) continue;
        if (count < 0) {
            perror("Epoll wait");
            break;
        }
            if (ev.data.fd == STDIN_FILENO) {
                fgets(buff, BUFSIZ, stdin);
                size_t len = strlen(buff);
                buff[len - 1] = 0;
                if (strcmp("off", buff) == 0) {
                    sock.status = OFF;
                }
                struct sockaddr_in from;
                bcast(sock, (void **) &buff, &size, NULL);
                printf("Send:%s\n", buff);
            }
        }
    free(buff);
    close(ep);
    close_bcast(sock);
    return EXIT_SUCCESS;
}