#ifndef ECHOAPP_MULTIPLEX_H
#define ECHOAPP_MULTIPLEX_H

#include "DefInfo.h"

typedef struct {
    int max;
    int fd;
    fd_set rfds;
    fd_set wfds;
    struct timeval time;
} select_t;

typedef struct {
    size_t count;
    size_t max;
    struct pollfd *pfd;
} poll_t;

#define START_EPOLL 5

void init_select(select_t *sel, __time_t sec, __suseconds_t susec);

int add_select(select_t *sel, int fd, int8_t type);

void rem_select(select_t *sel, int fd, int8_t type);

int wait_select(select_t *sel, select_t *out);

int check_select(select_t sel, int fd, int8_t type);

int init_poll(poll_t *pol, size_t max);

int add_poll(poll_t *pol, int sock);

void rem_poll(poll_t *pol, int fd);

int8_t wait_poll(poll_t *pol);

int8_t check_poll(poll_t *pol, int sock);

void free_poll(poll_t *pol);

#endif //ECHOAPP_MULTIPLEX_H
