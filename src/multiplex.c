#include "../headers/multiplex.h"

void init_select(select_t *sel, __time_t sec, __suseconds_t susec)
{
    sel->time = (struct timeval) {
        .tv_sec = sec,
        .tv_usec = susec
    };
    FD_ZERO(&(sel->rfds));
    FD_ZERO(&(sel->wfds));
    sel->max = 0;
}

int add_select(select_t *sel, int fd, int8_t type)
{
    if (fd > FD_SETSIZE) return EXIT_FAILURE;
    if (type == TYPE_READ) {
        FD_SET(fd, &sel->rfds);
    } else if (type == TYPE_WRITE) {
        FD_SET(fd, &sel->wfds);
    } else return EXIT_FAILURE;
    sel->max = (fd >= sel->max ? fd + 1 : sel->max);
    return EXIT_SUCCESS;
}
void rem_select(select_t *sel, int fd, int8_t type)
{
    switch (type) {
        case TYPE_READ:FD_CLR(fd, &sel->rfds);
            break;
        case TYPE_WRITE:FD_CLR(fd, &sel->wfds);
        default:break;
    }

    if (fd + 1 == sel->max && !FD_ISSET(fd, &sel->wfds) && !(FD_ISSET(fd, &sel->rfds))) --sel->max;
}

/*TIMEOUT; EXIT_FAILURE - error; EXIT_SUCCESS - set*/
int wait_select(select_t *sel, select_t *out)
{
    memcpy(out, sel, sizeof(*out));
    int res = select(sel->max, &out->rfds, &out->wfds, NULL, &out->time);
    if (res == 0) return TIMEOUT;
    if (res < 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int check_select(select_t sel, int fd, int8_t type)
{
    switch (type) {
        case TYPE_READ:if (FD_ISSET(fd, &sel.rfds)) return RREAD;
            break;
        case TYPE_WRITE:if (FD_ISSET(fd, &sel.wfds)) return RWRITE;
            break;
        default:break;
    }
    return EXIT_FAILURE;
}

int init_poll(poll_t *pol, size_t max)
{
    pol->max = max;
    pol->count = 0;
    pol->pfd = calloc(max, sizeof(struct pollfd));
    return (pol->pfd ? EXIT_SUCCESS : EXIT_FAILURE);
}

int add_poll(poll_t *pol, int sock)
{
    if (pol->count == pol->max) return EXIT_FAILURE;
    int pos = -1;
    for (int i = 0; i < pol->max; ++i) {
        if (pol->pfd[i].events == 0) {
            pos = i;
            break;
        }
    }
    if (pos < 0) return EXIT_FAILURE;
    pol->pfd[pos].fd = sock;
    pol->pfd[pos].events = POLLIN;
    pol->pfd[pos].revents = 0;
    ++pol->count;
    return EXIT_SUCCESS;
}

void rem_poll(poll_t *pol, int fd)
{
    size_t pos = pol->count - 1;
    for (int i = 0; i <= pos; ++i) {
        if (fd == pol->pfd[i].fd) {
            pos = (size_t) i;
            break;
        }
    }
    --pol->count;
    memmove(pol->pfd + pos, pol->pfd + pos + 1, sizeof(struct pollfd) * (pol->count - pos));
}
int8_t wait_poll(poll_t *pol)
{
    int ret = poll(pol->pfd, pol->count, 500);
    return (int8_t) (ret > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
int8_t check_poll(poll_t *pol, int sock)
{
    for (int i = 0; i < pol->count; ++i) {
        if (sock == pol->pfd[i].fd && pol->pfd[i].revents) {
            pol->pfd[i].revents = 0;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
void free_poll(poll_t *pol)
{
    if (pol->pfd) free(pol->pfd);
}






