#ifndef ECHOAPP_FRAMETCP_H
#define ECHOAPP_FRAMETCP_H

#include "DefInfo.h"

typedef struct {
    sock_t sock;
    int8_t status;
} server_tcp;

typedef struct {
    sock_t sock;
    void *buf;
    size_t size;
    struct sockaddr_in server;
    int8_t status;
} client_tcp;

#define ctcp_fd(cl) ((cl)->sock.fd)


int tcp_init_server(server_tcp *server, int domain, const char *ip, const uint16_t *port);

int serv_listen(server_tcp server, client_tcp *new_fd, size_t size);

int create_tcp_client(client_tcp *client, sock_t sock, size_t size);

int tcp_connect(client_tcp *client, char *ip, uint16_t port);

int tcp_recv(client_tcp *client);

int tcp_send(sock_t sock, void *msg, size_t size);

void *tcpMsg(client_tcp *client);

void close_client_tcp(client_tcp *client);

void close_server_tcp(server_tcp server);

void tcp_info_client(client_tcp *client, sinfo_t *info);

#endif //ECHOAPP_FRAMETCP_H
