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

void send_file(SOCKET client, const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        const char *err =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<h1>404 - Arquivo não encontrado</h1>";

        send(client, err, (int)strlen(err), 0);
        return;
    }

    const char *header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(client, header, (int)strlen(header), 0);

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        send(client, buffer, (int)bytes, 0);
    }

    fclose(file);
}

int main()
{
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[2048];

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
        printf("Erro ao inicializar Winsock\n");
        return 1;
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR_CHECK(server_fd))
    {
        perror("Erro ao criar socket");
        return 1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Erro no bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("Erro no listen");
        return 1;
    }

    printf("Servidor rodando em http://localhost:%d\n", PORT);

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (SOCKET_ERROR_CHECK(new_socket))
            continue;

        int received = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            CLOSESOCKET(new_socket);
            continue;
        }

        printf("Requisição:\n%s\n", buffer);

        if (strncmp(buffer, "GET /ping", 9) == 0)
        {
            send(new_socket, ping_response, (int)strlen(ping_response), 0);
        }
        else if (strncmp(buffer, "GET / HTTP", 10) == 0)
        {
            send_file(new_socket, "Front-end/index.html");
        }
        else if (strncmp(buffer, "GET /test", 9) == 0)
        {
            send_file(new_socket, "Front-end/test.html");
        }
        else if (strncmp(buffer, "GET /chat", 9) == 0)
        {
            send_file(new_socket, "Front-end/chat.html");
        }
        else if (strncmp(buffer, "GET /fruits", 11) == 0)
        {
            send_file(new_socket, "Front-end/fruits.html");
        }
        else
        {
            send_file(new_socket, "Front-end/index.html");
        }

        CLOSESOCKET(new_socket);
    }

    CLOSESOCKET(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
