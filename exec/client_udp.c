#include "../headers/FrameInet.h"
#include "../headers/multiplex.h"

static char ip[] = "127.0.0.1";
static uint16_t port = 7777;

int main(int argc, char **argv)
{
    sock_udp sock;
    sinfo_t me;
    if (udp_init(&sock)) {
        perror("Udp init client");
        return EXIT_FAILURE;
    }
    struct sockaddr_in to;
    init_addr(&to, AF_INET, ip, port);
    size_t size = BUFSIZ;
    char *buf = malloc(size);
    if (buf == NULL) {
        perror("Alloc buf");
        sock.status = OFF;
    }
    udp_send(&sock, &to, "I'm connected", 14);
    socket_info(sock.sock, &me);
    printf("Me %s:%d\n", me.ip, me.port);
    select_t set, out;
    init_select(&set, 1, 50);
    add_select(&set, sock.sock.fd, TYPE_READ);
    add_select(&set, STDIN_FILENO, TYPE_READ);
    while (sock.status) {
        int res = wait_select(&set, &out);
        if (res == TIMEOUT) continue;
        if (res == EXIT_FAILURE) {
            perror("Select error");
            sock.status = OFF;
            continue;
        }
        if (check_select(out, STDIN_FILENO, TYPE_READ) == RREAD) {
            memset(buf, 0, size);
            fgets(buf, (int) size, stdin);
            size_t len = strlen(buf);
            buf[len - 1] = '\0';
            if (strcmp("off", buf) == 0) {
                sock.status = OFF;
                strcpy(buf, "I'm disconnect");
            }
            if (udp_send(&sock, &to, buf, strlen(buf) + 1)) {
                printf("Server is closed\n");
                sock.status = OFF;
                break;
            }
        }
        if (check_select(out, sock.sock.fd, TYPE_READ) == RREAD) {
            size_t ret;
            struct sockaddr_in from;
            if ((ret = udp_recv(&sock, (void **) &buf, &size, &from)) == 0) {
                perror("Server is closed\n");
                sock.status = OFF;
                break;
            }
            get_addr(from, &me);
            printf("[%s:%d]: %s\n", me.ip, me.port, buf);
        }
    }

    free(buf);
    close_udp_sock(&sock);
    return EXIT_SUCCESS;
}