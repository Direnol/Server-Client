#include "../headers/FrameInet.h"


int init_server(server_t **server, sock_t *sock, size_t max_connect)
{
    server_t *new_server = malloc(sizeof(*new_server));
    if (new_server == NULL) return EXIT_FAILURE;
    new_server->sock = sock;
    if (bind(sock->fd_sock, (const struct sockaddr *) &sock->description, sock->socklen) < 0) {
        int err = errno;
        free(new_server);
        errno = err;
        return EXIT_FAILURE;
    }
    new_server->max_connection = max_connect;
    new_server->pfd = malloc(max_connect + 1);
    if (new_server->pfd == NULL) {
        free(new_server);
        return EXIT_FAILURE;
    }
    new_server->connections = malloc(sizeof(*sock) * max_connect);
    if (new_server->connections == NULL) {
        free(new_server->pfd);
        free(new_server);
        return EXIT_FAILURE;
    }
    listen(sock->fd_sock, 5);
    new_server->pfd[0] = (struct pollfd) {
        .fd = new_server->sock->fd_sock,
        .events = POLLIN | POLLRDNORM,
        .revents = 0
    };

    new_server->count_connection = 0;
    *server = new_server;
    return EXIT_SUCCESS;
}


void free_server(server_t *server)
{
    free_sock(server->sock);
    free(server->pfd);
    free(server->connections);
    free(server);
}

int listen_server(server_t *server)
{
    int ret = poll(server->pfd, server->count_connection + 1, 1000);
    if (ret < 0) return SERV_ERR;
    else if (ret == 0) return 0;

    if (ret > 1) ret = SERV_REQUEST;
    else ret = 0;

    if (server->pfd[0].revents & (POLLRDNORM | POLLIN)) ret |= NEW_CONNECT;
    else ret = SERV_REQUEST;

    return ret;
}

int accept_serv(server_t *server)
{
    if (server->count_connection == server->max_connection) return EXIT_FAILURE;
    size_t ind = (1 + server->count_connection);
    struct pollfd *connection = server->pfd + ind;
    server->connections[server->count_connection] = (sock_t) {
        .socklen = sizeof(struct sockaddr),
        .buf = NULL,
        .size_buf = 0,
        .fd_sock = -1
    };

    int new_fd = accept(server->sock->fd_sock,
                        (struct sockaddr *) &server->connections[server->count_connection].description,
                        &server->connections[server->count_connection].socklen);
    if (new_fd < 0) return EXIT_FAILURE;
    server->connections[server->count_connection].fd_sock = new_fd;

    connection->fd = new_fd;
    connection->events = POLLIN | POLLRDNORM;
    connection->revents = 0;

    ++server->count_connection;
    return EXIT_SUCCESS;
}