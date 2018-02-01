#include "../headers/FrameInet.h"
#include "../headers/DefInfo.h"

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
    while (client.status) {
        tcp_send(sock, "hey", 4);
        tcp_send(sock, "off", 4);
        tcp_recv(&client);
        printf("%s\n", (char *) tcpMsg(&client));
        client.status = OFF;
    }

    close_client_tcp(&client);
    return EXIT_SUCCESS;
}