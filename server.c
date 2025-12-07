#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define CLOSESOCKET closesocket
#define SOCKET_ERROR_CHECK(x) ((x) == SOCKET_ERROR)
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define CLOSESOCKET close
#define SOCKET int
#define SOCKET_ERROR_CHECK(x) ((x) < 0)
#endif

#define PORT 8080

int main()
{
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    const char *html_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<h1>Servidor HTTP funcionando!</h1>";

    const char *ping_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n"
        "{\"response\":\"pong\"}";

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "Falha ao inicializar Winsock\n");
        return 1;
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR_CHECK(server_fd))
    {
        perror("Erro ao criar socket");
        return 1;
    }

#ifndef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (SOCKET_ERROR_CHECK(bind(server_fd, (struct sockaddr *)&address, sizeof(address))))
    {
        perror("Erro no bind");
        CLOSESOCKET(server_fd);
        return 1;
    }

    if (SOCKET_ERROR_CHECK(listen(server_fd, 10)))
    {
        perror("Erro no listen");
        CLOSESOCKET(server_fd);
        return 1;
    }

    printf("Servidor rodando em http://localhost:%d\n", PORT);

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (SOCKET_ERROR_CHECK(new_socket))
        {
            perror("Erro no accept");
            continue;
        }

        recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        printf("Requisição recebida:\n%s\n", buffer);

        if (strncmp(buffer, "GET /ping", 9) == 0)
        {
            send(new_socket, ping_response, strlen(ping_response), 0);
        }
        else
        {
            send(new_socket, html_response, strlen(html_response), 0);
        }

        CLOSESOCKET(new_socket);
    }

    CLOSESOCKET(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
