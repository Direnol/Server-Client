#include "../headers/FrameInet.h"
#include "../headers/multiplex.h"

static char ip[] = "127.0.0.1";
static uint16_t port = 7777;

int main(int argc, char **argv)
{
    server_tcp server;
    sinfo_t info;
    if (tcp_init_server(&server, AF_INET, ip, &port)) {
        perror("Tcp server init");
        return EXIT_FAILURE;
    }
    socket_info(server.sock, &info);
    printf("Server is started ==> %s:%d\n", info.ip, info.port);
    int set = epoll_create(START_EPOLL);

    if (set < 0) {
        perror("Epoll create");
        server.status = OFF;
    } else {
        struct epoll_event ev = {
            .events = EPOLLIN,
            .data.fd = -2
        };
        if (epoll_ctl(set, EPOLL_CTL_ADD, server.sock.fd, &ev)) {
            server.status = OFF;
        }
        ev.data.fd = -1;
        if (epoll_ctl(set, EPOLL_CTL_ADD, STDIN_FILENO, &ev)) {
            server.status = OFF;
        }
    }
    size_t size_buff = START_EPOLL;
    char buf[BUFSIZ];
    struct epoll_event *ev = malloc(sizeof(*ev) * START_EPOLL);
    client_tcp *client = calloc(START_EPOLL, sizeof(*client));
    if (ev == NULL || client == NULL) server.status = OFF;
    else {
        client[0].sock.fd = STDIN_FILENO;
        client[1].sock.fd = server.sock.fd;
    }
    int count_connection = 2;
    while (server.status) {
        int res = epoll_wait(set, ev, count_connection, 3000);
        if (res == 0) continue;
        if (res < 0) {
            perror("Epoll wait");
            server.status = OFF;
            continue;
        }
        size_t len = 0;
        int8_t send_flg = OFF;
        for (int i = 0; i < res; ++i) {
            if (ev[i].data.fd == -1) {
                memset(buf, 0, BUFSIZ);
                fgets(buf, BUFSIZ - LEN_MSG_SERV, stdin);
                len = strlen(buf);
                buf[len - 1] = '\0';
                if (strcmp("off", buf) == 0) {
                    server.status = OFF;
                    break;
                } else {
                    send_flg = ON;
                    strcat(buf, MSG_SERV);
                    len += LEN_MSG_SERV - 1;
                }
                break;
            } else if (ev[i].data.fd == -2) {
                int pos = tcp_first_free(client, size_buff);
                if (serv_listen(server, &client[pos], 4)) {
                    perror("Accept client");
                    server.status = OFF;
                    continue;
                }
                struct epoll_event _ev = {
                    .events = EPOLLIN,
                    .data.fd = pos
                };
                if (epoll_ctl(set, EPOLL_CTL_ADD, client[i].sock.fd, &_ev)) {
                    perror("Epoll add new connection");
                    server.status = OFF;
                    break;
                } else {
                    ++count_connection;
                    if (count_connection > size_buff) {
                        size_buff <<= 1;
                        ev = realloc(ev, sizeof(*ev) * size_buff);
                        client = realloc(client, sizeof(*client) * size_buff);
                        if (ev == NULL || client == NULL) {
                            perror("New buf");
                            server.status = OFF;
                            exit(EXIT_FAILURE);
                        }
                    }
                    tcp_info_client(&client[pos], &info);
                    printf("Log: new connection [%s:%d] %d users\n", info.ip, info.port,
                           count_connection - 2);
                }
            } else {
                int pos = ev[i].data.fd;
                tcp_info_client(&client[pos], &info);

                if (tcp_recv(&client[pos])) continue;

                if (strcmp(tcpMsg(&client[pos]), "off") == 0) {
                    epoll_ctl(set, EPOLL_CTL_DEL, client[pos].sock.fd, NULL);
                    close_client_tcp(&client[pos]);
                    --count_connection;
                    if (count_connection == 2) {
                        server.status = OFF;
                    }
                    printf("Log[%s:%d]: disconnect\nLog %d users\n", info.ip, info.port,
                           count_connection - 2);
                    continue;
                }

                char msg[client[pos].size + 15];
                strcpy(msg, tcpMsg(&client[pos]));
                printf("Log[%s:%d]: %s\n", info.ip, info.port, msg);

                strcat(msg, " \"Server\"");
                tcp_send(client[pos].sock, msg, strlen(msg) + 1);
            }
        }
        for (int i = 0; i < size_buff && send_flg == ON; ++i) {
            if (client[i].status == OFF) continue;
            tcp_info_client(client + i, &info);
            int ret = tcp_send(client[i].sock, buf, len);
            if (ret) {
                fprintf(stderr, "Cann't send to [%s:%d]: %s\n", info.ip, info.port,
                        strerror(errno));
            } else {
                printf("Log: [%s] to [%s:%d]\n", buf, info.ip, info.port);
            }

        }
    }
    for (int i = 0; i < size_buff; ++i) {
        if (client[i].status) close_client_tcp(client + i);
    }
    if (client) free(client);
    if (ev) free(ev);
    if (set >= 0)
        close(set);
    close_server_tcp(server);
    return EXIT_SUCCESS;
}