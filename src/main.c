#include <stdio.h>
#include "../headers/FrameInet.h"

int main()
{
    sock_t *sock = NULL;
    struct sockaddr_in serv_in = sock_in(AF_INET, 7777, "127.0.0.1");
    if (init_sock(&sock, SOCK_STREAM, 64, &serv_in)) {
        perror("Init");
        exit(EXIT_FAILURE);
    }

    free_sock(sock);
    return 0;
}