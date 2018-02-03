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

#define MAX_CLIENTS (FD_SETSIZE - 2)
#define MSG_SERV (" \"Server\"")
#define LEN_MSG_SERV (sizeof(MSG_SERV))

#define TIMEOUT (-5)
#define RREAD (-6)
#define RWRITE (-7)

#define TYPE_READ 1
#define TYPE_WRITE 0

typedef struct {
    int fd;
    struct sockaddr_in sockaddrIn;
    socklen_t socklen;
} sock_t;

typedef struct {
    uint16_t port;
    char ip[INET_ADDRSTRLEN];
} sinfo_t;


typedef struct {

} broadcast_t;

#endif //ECHOAPP_DEFINFO_H
