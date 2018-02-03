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

    poll_t set;
    init_poll(&set, MAX_CLIENTS);

    client_tcp *client = calloc(MAX_CLIENTS, sizeof(*client));
    if (!client) {
        perror("Alloc clients");
        server.status = OFF;
    }
    add_poll(&set, server.sock.fd);
    add_poll(&set, STDIN_FILENO);
    int count_connection = 0;
    char buf[BUFSIZ];
    while (server.status) {
        int res = wait_poll(&set);
        if (res) continue;
        if (check_poll(&set, server.sock.fd) == EXIT_SUCCESS) {
            int pos = tcp_first_free(client, MAX_CLIENTS);
            if (pos >= 0) {
                if (serv_listen(server, client + pos, 4)) {
                    perror("Accept client");
                    server.status = OFF;
                    continue;
                }
                ++count_connection;
                if (add_poll(&set, client[pos].sock.fd)) {
                    perror("Add select new client");
                    server.status = OFF;
                    continue;
                }
                tcp_info_client(&client[pos], &info);
                printf("Log: new connection [%s:%d] %d from %d users\n", info.ip, info.port,
                       count_connection, MAX_CLIENTS);
            }
        }

        int8_t send_flg = OFF;
        size_t len = 0;
        if (check_poll(&set, STDIN_FILENO) == EXIT_SUCCESS) {
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
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client[i].status == OFF) continue;
            tcp_info_client(client + i, &info);
            if (send_flg) {
                int ret = tcp_send(client[i].sock, buf, len);
                if (ret) {
                    fprintf(stderr, "Cann't send to [%s:%d]: %s\n", info.ip, info.port,
                            strerror(errno));
                } else {
                    printf("Log: [%s] to [%s:%d]\n", buf, info.ip, info.port);
                }
            }
            int check = check_poll(&set, client[i].sock.fd);
            if (check) continue;
            if (tcp_recv(&client[i])) continue;

            if (strcmp(tcpMsg(&client[i]), "off") == 0) {
                close_client_tcp(&client[i]);
                --count_connection;
                rem_poll(&set, client[i].sock.fd);
                if (!count_connection) {
                    server.status = OFF;
                }
                printf("Log[%s:%d]: disconnect\nLog %d from %d users\n", info.ip, info.port,
                       count_connection, MAX_CLIENTS);
                continue;
            }

            char msg[client[i].size + 15];
            strcpy(msg, tcpMsg(&client[i]));
            printf("Log[%s:%d]: %s\n", info.ip, info.port, msg);

            strcat(msg, " \"Server\"");
            tcp_send(client[i].sock, msg, strlen(msg) + 1);
        }
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client[i].status)
            close_client_tcp(&client[i]);
    }
    free(client);
    close_server_tcp(server);
    free_poll(&set);
    return EXIT_SUCCESS;
}