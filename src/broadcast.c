#include <ifaddrs.h>
#include <net/if.h>
#include "../headers/Broadcast.h"
#include "../headers/FrameUdp.h"
#include "../headers/DefInfo.h"

int init_bcast(broadcast_t *sock, int8_t type, char *ip, char *mask, uint16_t port)
{
    struct sockaddr_in binfo;
    memset(binfo.sin_zero, 0, sizeof(sock->info.sin_zero));
    binfo.sin_addr.s_addr = get_bcast_addr(ip, mask);
    binfo.sin_port = htons(port);
    binfo.sin_family = AF_INET;
    sock_t _sock;
    if (create_sock(&_sock, AF_INET, SOCK_DGRAM)) {
        return EXIT_FAILURE;
    }
    memcpy(&sock->sock.sock, &_sock, sizeof(_sock));
    int val = 1;
    switch (type) {
        case BSERV:
            if (setsockopt(sock->sock.sock.fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val)) < 0) {
                close_udp_sock(&sock->sock);
                return EXIT_FAILURE;
            }
            memcpy(&sock->info, &binfo, sizeof(binfo));
            break;

        case BCLIENT:
            memcpy(&sock->sock.sock.sockaddrIn, &binfo, sizeof(binfo));
            if (bind(sock->sock.sock.fd, (const struct sockaddr *) &binfo, sizeof(binfo)) < 0) {
                close_udp_sock(&sock->sock);
                return EXIT_FAILURE;
            }
            break;
        default:return EXIT_FAILURE;
    }
    sock->type = type;
    sock->status = ON;
    return EXIT_SUCCESS;
}

uint32_t get_bcast_addr(char *ip, char *mask)
{

    return inet_addr("192.168.1.255");
}

void close_bcast(broadcast_t sock)
{
    close_udp_sock(&sock.sock);
}
int bcast(broadcast_t sock, void **msg, size_t *n, struct sockaddr_in *from)
{
    struct sockaddr_in _fr;
    switch (sock.type) {
        case BSERV:
            udp_send(&sock.sock, &sock.info, *msg, *n);
            break;
        case BCLIENT:
            udp_recv(&sock.sock, msg, n, &_fr);
            if (from) {
                memcpy(from, &_fr, sizeof(*from));
            }
            break;
        default: return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
