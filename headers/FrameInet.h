#ifndef ECHOAPP_FRAMESOCK_H
#define ECHOAPP_FRAMESOCK_H

#include "FrameTcp.h"


int create_sock(sock_t *sock, int domain, int type);

int setIP(sock_t *sock, const char *ip, const uint16_t *port);

int socket_info(sock_t sock, sinfo_t *info);

void closeSock(sock_t sock);



#endif //ECHOAPP_FRAMESOCK_H
