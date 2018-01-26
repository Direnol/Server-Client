#ifndef ECHOAPP_FRAMESOCK_H
#define ECHOAPP_FRAMESOCK_H

#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define sock_in(family, port, addr) { \
    .sin_family = (family), \
    .sin_port = htons((port)), \
    .sin_addr.s_addr = inet_addr(addr) \
}

struct frame_sock {
    int fd_sock;
    struct sockaddr_in description;
    void *buf;
    size_t size_buf;
    socklen_t socklen;
};

typedef struct frame_sock sock_t;

#define NEW_CONNECT (1)
#define SERV_REQUEST (2)
#define SERV_CLOSE (-2)
#define SERV_ERR (-3)

struct frame_server {
    sock_t *sock;
    sock_t *connections;
    nfds_t count_connection;
    nfds_t max_connection;
    struct pollfd *pfd;
};

typedef struct frame_server server_t;

int init_sock(sock_t **sock, int sock_type, size_t size_buf, struct sockaddr_in *desc);

void free_sock(sock_t *sock);

int connect_sock(sock_t *sock);

int init_server(server_t **server, sock_t *sock, nfds_t max_connect);

int put_msg(sock_t *sock, void *msg, size_t n);

void *get_msg(sock_t *sock, size_t *n);

void free_server(server_t *server);

/* -3 - error; -2 - server is closed; 1 - new connection; 0 - timer; 2 - request */
int listen_server(server_t *server);

int accept_serv(server_t *server);

int recv_msg(sock_t *sock);

int send_msg(sock_t *sock);

#endif //ECHOAPP_FRAMESOCK_H
