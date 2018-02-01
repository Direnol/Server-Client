#include "../headers/FrameInet.h"

int create_sock(sock_t *sock, int domain, int type)
{
    sock->fd = socket(domain, type, 0);
    if (sock->fd < 0)
        return EXIT_FAILURE;
    sock->socklen = sizeof(sock->sockaddrIn);
    if (getsockname(sock->fd, (struct sockaddr *) &sock->sockaddrIn, &sock->socklen) < 0)
        return EXIT_FAILURE;
    int on = 1;
    if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int setIP(sock_t *sock, const char *ip, const uint16_t *port)
{
    struct sockaddr_in new_addr;
    memset(&new_addr, 0, sizeof(new_addr));
    uint16_t _port = (port ? htons(*port) : sock->sockaddrIn.sin_port);
    in_addr_t _ip = (ip ? inet_addr(ip) : sock->sockaddrIn.sin_addr.s_addr);
    new_addr = (struct sockaddr_in) {
        .sin_family = sock->sockaddrIn.sin_family,
        .sin_addr.s_addr = _ip,
        .sin_port = _port
    };
    if (bind(sock->fd, (const struct sockaddr *) &new_addr, sock->socklen) < 0)
        return EXIT_FAILURE;
    memcpy(&sock->sockaddrIn, &new_addr, sizeof(new_addr));
    return EXIT_SUCCESS;
}

void closeSock(sock_t sock)
{
    close(sock.fd);
}
int socket_info(sock_t sock, sinfo_t *info)
{
    memset(info, 0, sizeof(*info));
    struct sockaddr_in _info;
    socklen_t socklen = sizeof(_info);
    memset(&_info, 0, socklen);
    if (getsockname(sock.fd, (struct sockaddr *) &_info, &socklen) < 0) return EXIT_FAILURE;
    info->port = ntohs(_info.sin_port);
    char *ip = inet_ntoa(_info.sin_addr);
    memcpy(info->ip, ip, strlen(ip) + 1);
    return EXIT_SUCCESS;
}
