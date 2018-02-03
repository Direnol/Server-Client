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

    select_t set;
    init_select(&set, 5, 0);

    client_tcp *client = calloc(MAX_CLIENTS, sizeof(*client));
    if (!client) {
        perror("Alloc clients");
        server.status = OFF;
    }
    add_select(&set, server.sock.fd, TYPE_READ);
    add_select(&set, STDIN_FILENO, TYPE_READ);
    select_t out;
    int count_connection = 0;
    int8_t err = 0;
    char buf[BUFSIZ];
    while (server.status) {
        int res = wait_select(&set, &out);
        if (res == TIMEOUT) continue;
        if (res == EXIT_FAILURE) {
            ++err;
            perror("Select server");
            continue;
        }
        if (err == 5) {
            server.status = OFF;
            continue;
        }
        err = 0;
        if (check_select(out, server.sock.fd, TYPE_READ) == RREAD) {
            int pos = tcp_first_free(client, MAX_CLIENTS);
            if (pos >= 0) {
                if (serv_listen(server, client + pos, 4)) {
                    perror("Accept client");
                    server.status = OFF;
                    continue;
                }
                ++count_connection;
                if (add_select(&set, client[pos].sock.fd, TYPE_READ)) {
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
        if (check_select(out, STDIN_FILENO, TYPE_READ) == RREAD) {
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
            int check = check_select(out, client[i].sock.fd, TYPE_READ);
            if (check != RREAD) continue;
            if (tcp_recv(&client[i])) continue;

            if (strcmp(tcpMsg(&client[i]), "off") == 0) {
                close_client_tcp(&client[i]);
                --count_connection;
                rem_select(&set, client[i].sock.fd, TYPE_READ);
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
    return EXIT_SUCCESS;
}