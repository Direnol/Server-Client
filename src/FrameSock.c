#include "../headers/FrameInet.h"

int init_sock(sock_t **sock, int sock_type, size_t size_buf, struct sockaddr_in *desc)
{
    sock_t *new_sock = malloc(sizeof(*new_sock));
    if (new_sock == NULL) return EXIT_FAILURE;
    if (size_buf) {
        new_sock->size_buf = size_buf;
        new_sock->buf = malloc(size_buf);
        if (new_sock->buf == NULL) {
            free(new_sock);
            return EXIT_FAILURE;
        }
    } else {
        new_sock->size_buf = 0;
        new_sock->buf = NULL;
    }
    new_sock->fd_sock = socket(AF_INET, sock_type, 0);
    if (new_sock->fd_sock < 0) {
        int err = errno;
        if (new_sock->buf) {
            free(new_sock->buf);
        }
        free(new_sock);
        errno = err;
        return EXIT_FAILURE;
    }
    new_sock->socklen = sizeof(*new_sock);
    memcpy(&new_sock->description, desc, sizeof(*desc));
    *sock = new_sock;
    return EXIT_SUCCESS;
}

void free_sock(sock_t *sock)
{
    if (sock->buf) {
        free(sock->buf);
    }
    free(sock);
}

int connect_sock(sock_t *sock)
{
    if (sock == NULL) return EXIT_FAILURE;
    if (connect(sock->fd_sock, (const struct sockaddr *) &sock->description, sock->socklen) < 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int put_msg(sock_t *sock, void *msg, size_t n)
{
    if (sock->size_buf < n) {
        sock->buf = realloc(sock->buf, n);
        if (sock->buf == NULL) return EXIT_FAILURE;
    }
    memcpy(sock->buf, msg, n);
    return EXIT_SUCCESS;
}

void *get_msg(sock_t *sock, size_t *n)
{
    if (n != NULL) {
        *n = sock->size_buf;
    }
    return sock->buf;
}

int recv_msg(sock_t *sock)
{
    // TODO MSG_PEEK
    ssize_t n = recvfrom(sock->fd_sock, sock->buf, sock->size_buf, 0, (struct sockaddr *) &sock->description, &sock->socklen);
    if (n <= 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int send_msg(sock_t *sock)
{
    ssize_t  n = sendto(sock->fd_sock, sock->buf, sock->size_buf, 0, (struct sockaddr *) &sock->description, sock->socklen);
    if (n <= 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}