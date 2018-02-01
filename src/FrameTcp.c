#include "../headers/FrameInet.h"
#include "../headers/DefInfo.h"
#include "../headers/FrameTcp.h"

int tcp_init_server(server_tcp *server, int domain, const char *ip, const uint16_t *port)
{
    if (create_sock(&server->sock, domain, SOCK_STREAM)) {
        perror("Create sock");
        return EXIT_FAILURE;
    }
    if (setIP(&server->sock, ip, port)) {
        perror("Set ip:port");
        return EXIT_FAILURE;
    }
    if (listen(server->sock.fd, 5) < 0)
        return EXIT_FAILURE;
    server->status = ON;

    return EXIT_SUCCESS;
}

int serv_listen(server_tcp server, client_tcp *new_fd, size_t size)
{
    sock_t sock;
    sock.socklen = sizeof(struct sockaddr_in);
    sock.fd = accept(server.sock.fd, (struct sockaddr *) &sock.sockaddrIn, &sock.socklen);
    if (sock.fd < 0) return EXIT_FAILURE;

    return create_tcp_client(new_fd, sock, size);
}

int create_tcp_client(client_tcp *client, sock_t sock, size_t size)
{
    memcpy(&client->sock, &sock, sizeof(sock));
    client->size = size < sizeof(size_t) ? sizeof(size_t) : size;
    client->buf = malloc(client->size);
    if (client->buf == NULL) return EXIT_FAILURE;
    client->status = ON;
    return EXIT_SUCCESS;
}

void close_client_tcp(client_tcp *client)
{
    closeSock(client->sock);
    if (client->buf) free(client->buf);
}

void close_server_tcp(server_tcp server)
{
    closeSock(server.sock);
}

int tcp_recv(client_tcp *client)
{
    ssize_t res = recv(ctcp_fd(client), client->buf, sizeof(size_t), 0);
    if (res <= 0) return EXIT_FAILURE;
    size_t size = *((size_t *) client->buf);
    if (client->size < size) {
        client->size = size;
        client->buf = realloc(client->buf, size);
    }
    if (client->buf == NULL) return EXIT_FAILURE;
    res = recv(ctcp_fd(client), client->buf, size, 0);
    if (res <= 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int tcp_send(sock_t sock, void *msg, size_t size)
{
    ssize_t res = -1;
    res = send(sock.fd, &size, sizeof(size), 0);
    if (res <= 0) return EXIT_FAILURE;
    res = send(sock.fd, msg, size, 0);
    if (res <= 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void *tcpMsg(client_tcp *client)
{
    return client->buf;
}
int tcp_connect(client_tcp *client, char *ip, uint16_t port)
{
    client->server = (struct sockaddr_in) {
        .sin_port = htons(port),
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(ip)
    };
    int res = connect(ctcp_fd(client), (const struct sockaddr *) &client->server,
                      sizeof(client->server));
    if (res < 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void tcp_info_client(client_tcp *client, sinfo_t *info)
{
    info->port = ntohs(client->sock.sockaddrIn.sin_port);
    char *ip = inet_ntoa(client->sock.sockaddrIn.sin_addr);
}
