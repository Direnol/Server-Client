#include "../headers/FrameUdp.h"
#include "../headers/DefInfo.h"

int udp_init(sock_udp *sock)
{
    int res = create_sock(&sock->sock, AF_INET, SOCK_DGRAM);
    if (res) return res;
    sock->status = ON;
    return EXIT_SUCCESS;
}

int udp_bind(sock_udp *sock, char *ip, uint16_t *port)
{
    return setIP(&sock->sock, ip, port);
}

int udp_send(sock_udp *sock, struct sockaddr_in *to, void *msg, size_t n)
{
    socklen_t socklen = sizeof(struct sockaddr_in);
    ssize_t ret = sendto(sock->sock.fd, &n, sizeof(n), 0, (const struct sockaddr *) to, socklen);
    if (ret <= 0) return EXIT_FAILURE;
    ret = sendto(sock->sock.fd, msg, n, 0, (const struct sockaddr *) to, socklen);
    return ret <= 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

size_t udp_recv(sock_udp *sock, void **buf, size_t *n, struct sockaddr_in *from)
{
    size_t nrecv = 0;
    memset(from, 0, sizeof(*from));
    socklen_t socklen = sizeof(struct sockaddr_in);
    if (recvfrom(sock->sock.fd, &nrecv, sizeof(size_t), 0, (struct sockaddr *) from, &socklen) <= 0)
        return 0;
    if (*n < nrecv) {
        *n = nrecv;
        *buf = realloc(*buf, *n);
    }
    if (recvfrom(sock->sock.fd, *buf, nrecv, 0, (struct sockaddr *) from,
                 &socklen) <= 0)
        return EXIT_FAILURE;

    return nrecv;
}

void close_udp_sock(sock_udp *sock)
{
    closeSock(sock->sock);
    sock->status = OFF;
}
