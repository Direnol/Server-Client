#ifndef ECHOAPP_FRAMEUDP_H
#define ECHOAPP_FRAMEUDP_H

#include "FrameInet.h"

typedef struct {
    sock_t sock;
    int8_t status;
} sock_udp;

int udp_init(sock_udp *sock);

int udp_bind(sock_udp *sock, char *ip, uint16_t *port);

int udp_send(sock_udp *sock, struct sockaddr_in *to, void *msg, size_t n);

size_t udp_recv(sock_udp *sock, void **buf, size_t *n, struct sockaddr_in *from);

void close_udp_sock(sock_udp *sock);

#endif //ECHOAPP_FRAMEUDP_H
