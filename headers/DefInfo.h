#ifndef ECHOAPP_DEFINFO_H
#define ECHOAPP_DEFINFO_H

#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define ON 1
#define OFF 0


typedef struct {
    int fd;
    struct sockaddr_in sockaddrIn;
    socklen_t socklen;
} sock_t;

typedef struct {
    uint16_t port;
    char ip[3 * 4 + 3];
} sinfo_t;


typedef struct {

} server_udp;

typedef struct {

} client_udp;

typedef struct {

} broadcast_t;

#endif //ECHOAPP_DEFINFO_H
