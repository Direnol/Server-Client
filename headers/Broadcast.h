#ifndef ECHOAPP_BROADCAST_H
#define ECHOAPP_BROADCAST_H

#include "FrameUdp.h"

typedef struct {
    sock_udp sock;
    int8_t type;
    int8_t status;
    struct sockaddr_in info;
} broadcast_t;

#define BSERV 1
#define BCLIENT 2

int init_bcast(broadcast_t *sock, int8_t type, char *ip, char *mask, uint16_t port);

int bcast(broadcast_t sock, void **msg, size_t *n, struct sockaddr_in *from);

uint32_t get_bcast_addr(char *ip, char *mask);

void close_bcast(broadcast_t sock);

#endif //ECHOAPP_BROADCAST_H
