#include <stdio.h>
#include "../headers/FrameInet.h"

int main(int argc, char **argv)
{
    server_tcp server;
    char ip[] = "127.0.0.1";
    uint16_t port = 7777;
    if (tcp_init_server(&server, AF_INET, ip, &port)) {
        perror("Tcp init");
        return EXIT_FAILURE;
    }
    sinfo_t info;
    socket_info(server.sock, &info);
    puts("Server is started");
    printf("%s:%d\n", info.ip, info.port);

    client_tcp client;
    if (serv_listen(server, &client, 0)) {
        fprintf(stderr, "Error connect\n");
        server.status = OFF;
    }
    tcp_info_client(&client, &info);
    printf("Client %s:%d\n", info.ip, info.port);
    while (server.status) {
        if (tcp_recv(&client)) continue;
        if (strcmp(tcpMsg(&client), "off") == 0) {
            server.status = OFF;
        }
        
        char msg[client.size + 15];
        strcpy(msg, tcpMsg(&client));
        printf("Log: %s\n", msg);
        strcat(msg, " \"Server\"\n");
        tcp_send(client.sock, msg, strlen(msg) + 1);
        
    }
    close_client_tcp(&client);
    close_server_tcp(server);
    return EXIT_SUCCESS;
}