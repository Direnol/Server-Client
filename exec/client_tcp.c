#include "../headers/FrameInet.h"
#include "../headers/multiplex.h"

int main(int argc, char **argv)
{
    sock_t sock;
    sinfo_t me;
    if (create_sock(&sock, AF_INET, SOCK_STREAM)) {
        perror("Create sock");
        return EXIT_FAILURE;
    }
    client_tcp client;
    if (create_tcp_client(&client, sock, 0)) {
        perror("Create client");
        client.status = OFF;
    }
    if (tcp_connect(&client, "127.0.0.1", 7777)) {
        perror("Connect");
        client.status = OFF;
    }
    socket_info(sock, &me);
    printf("%s:%d\n", me.ip, me.port);
    char buf[BUFSIZ];
    select_t set, out;
    init_select(&set, 1, 50);
    add_select(&set, sock.fd, TYPE_READ);
    add_select(&set, STDIN_FILENO, TYPE_READ);
    while (client.status) {
        int res = wait_select(&set, &out);
        if (res == TIMEOUT) continue;
        if (res == EXIT_FAILURE) {
            perror("Select error");
            client.status = OFF;
            continue;
        }
        if (check_select(out, STDIN_FILENO, TYPE_READ) == RREAD) {
            memset(buf, 0, BUFSIZ);
            fgets(buf, BUFSIZ - 1, stdin);
            size_t len = strlen(buf);
            buf[len - 1] = '\0';
            if (strcmp("off", buf) == 0) client.status = OFF;
            if (tcp_send(client.sock, buf, len)) {
                printf("Server is closed\n");
                client.status = OFF;
                break;
            }
        }
        if (check_select(out, sock.fd, TYPE_READ) == RREAD) {
            if (tcp_recv(&client)) {
                perror("Server is closed\n");
                client.status = OFF;
                break;
            }
            printf("%s\n", (char *) tcpMsg(&client));
        }
    }

    close_client_tcp(&client);
    return EXIT_SUCCESS;
}