#include "../headers/FrameInet.h"
#include "../headers/FrameUdp.h"
#include "../headers/DefInfo.h"

static char ip[] = "127.0.0.1";
static uint16_t port = 7777;

int main(int argc, char **argv)
{
    sock_udp sock;
    if (udp_init(&sock)) {
        perror("Init udp server");
        return EXIT_FAILURE;
    }
    if (udp_bind(&sock, ip, &port)) {
        perror("Bind udp server");
        sock.status = OFF;
    }
    int epl = epoll_create(2);
    struct epoll_event *ev = malloc(2 * sizeof(*ev));
    if (ev == NULL) {
        perror("Malloc events");
        sock.status = OFF;
    } else {
        ev[0] = (struct epoll_event) {
            .events = EPOLLIN,
            .data.fd = STDIN_FILENO
        };
        ev[1] = (struct epoll_event) {
            .events = EPOLLIN,
            .data.fd = sock.sock.fd
        };
        if (epoll_ctl(epl, EPOLL_CTL_ADD, STDIN_FILENO, ev) < 0) {
            perror("Epoll add stdin");
            sock.status = OFF;
        }
        if (epoll_ctl(epl, EPOLL_CTL_ADD, sock.sock.fd, ev + 1) < 0) {
            perror("Epoll add server");
            sock.status = OFF;
        }
    }
    char *buf = malloc(BUFSIZ);
    size_t size = BUFSIZ;
    struct sockaddr_in from;
    sinfo_t info;
    socket_info(sock.sock, &info);
    if (sock.status) {
        printf("Server is started [%s:%d]\n", info.ip, info.port);
    }
    while (sock.status) {
        int res = epoll_wait(epl, ev, 2, 3000);
        if (res == 0) continue;
        if (res < 0) {
            perror("Epoll wait");
            sock.status = OFF;
            break;
        }
        for (int i = 0; i < res; ++i) {
            if (ev[i].data.fd == STDIN_FILENO) {
                fgets(buf, (int) size, stdin);
                buf[strlen(buf) - 1] = '\0';
                if (strcmp(buf, "off") == 0) {
                    sock.status = OFF;
                }
            } else if (ev[i].data.fd == sock.sock.fd) {
                size_t ret = 0;
                if ((ret = udp_recv(&sock, (void **) &buf, &size, &from)) == 0) {
                    perror("Recv udp");
                    sock.status = OFF;
                    break;
                }
                get_addr(from, &info);
                printf("Log[%s:%d]: %s\n", info.ip, info.port, buf);
                if (ret + LEN_MSG_SERV < size) {
                    size += LEN_MSG_SERV;
                    buf = realloc(buf, size);
                }
                strcat(buf, MSG_SERV);
                if (udp_send(&sock, &from, buf, strlen(buf) + 1)) {
                    perror("Send udp");
                    sock.status = OFF;
                    break;
                }
            } else {
                fprintf(stderr, "Incorrect fd");
                sock.status = OFF;
                break;
            }
        }
    }
    free(buf);
    close_udp_sock(&sock);
    return EXIT_SUCCESS;
}