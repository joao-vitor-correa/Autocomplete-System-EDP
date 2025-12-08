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

#include "autocomplete.h"

#define PORT 8080
#define TAM_MAX 128

// ========================
// BASE DE DADOS GLOBAL (servidor controla a raiz)
// ========================
NoAVL *raiz = NULL;

void carregar_dados()
{
    const char *lista[] = {
        "abacate", "abacaxi", "abrico", "acai", "acerola", "amora", "pera",
        "ameixa", "araca", "araticum", "bacaba", "bacuri", "banana",
        "buriti", "caja", "caju", "camucamu", "carambola", "ciriguela",
        "cupuaçu", "damasco", "figo",
        "framboesa-do-mato", "goiaba", "graviola", "groselha",
        "jabuticaba", "jaca", "jambo", "jenipapo", "jiricó",
        "laranja-lima", "limao-cravo", "mamao",
        "manga", "maracuja", "murici", "umbu", "uvaia",
        "pitanga", "pitaya-brasileira", "pequi", "pinha",
        "seriguela", "tamarindo", "tucumã", "uva-do-japao"};

    int total = sizeof(lista) / sizeof(lista[0]);

    for (int i = 0; i < total; i++)
        raiz = inserirAVL(raiz, lista[i]);
}

// ========================
// ENVIO DE ARQUIVOS HTML
// ========================
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

// ========================
// MAIN SERVIDOR
// ========================
int main()
{
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[4096];

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

    carregar_dados(); // <-- inicializa o autocomplete

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

        // Imprime o request inteiro para debug
        printf("=== REQUEST BEGIN ===\n%s\n=== REQUEST END ===\n", buffer);

        // ========================
        // ROTAS
        // ========================

        if (strncmp(buffer, "GET /ping", strlen("GET /ping")) == 0)
        {
            send(new_socket, ping_response, (int)strlen(ping_response), 0);
        }
        // ROTA DE DEBUG: /debbugin
        else if (strncmp(buffer, "GET /debbugin?term=", strlen("GET /debbugin?term=")) == 0)
        {
            char termo[TAM_MAX] = {0};
            // lê até o espaço (HTTP/1.1)
            sscanf(buffer, "GET /debbugin?term=%127[^ ]", termo);

            // imprime no servidor (debug)
            printf("[DEBUG] rota /debbugin acionada\n");
            printf("[DEBUG] termo parseado: '%s'\n", termo);

            char resultados[3][TAM_MAX];
            int qtd = autocompleteAVL(raiz, termo, resultados);

            // montar JSON de debug com raw_request (tamanho limitado)
            char raw_clip[1024] = {0};
            strncpy(raw_clip, buffer, sizeof(raw_clip) - 1);

            // calcular tamanho suficiente para resposta
            char resposta[2048];
            int pos = 0;

            pos += snprintf(resposta + pos, sizeof(resposta) - pos,
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "{ \"route\": \"/debbugin\", \"term\": \"%s\", \"qtd\": %d, \"resultados\": [",
                            termo, qtd);

            for (int i = 0; i < qtd; i++)
            {
                pos += snprintf(resposta + pos, sizeof(resposta) - pos,
                                "\"%s\"%s", resultados[i], (i < qtd - 1) ? "," : "");
            }

            pos += snprintf(resposta + pos, sizeof(resposta) - pos,
                            "], \"raw_request\": \"%s\" }",
                            raw_clip);

            send(new_socket, resposta, (int)strlen(resposta), 0);
        }
        else if (strncmp(buffer, "GET /autocomplete?term=", strlen("GET /autocomplete?term=")) == 0)
        {
            // ainda aceitamos a rota antiga caso precise testar
            char termo[TAM_MAX] = {0};
            sscanf(buffer, "GET /autocomplete?term=%127[^ ]", termo);
            printf("[DEBUG] rota /autocomplete acionada (fallback). termo='%s'\n", termo);

            char resultados[3][TAM_MAX];
            int qtd = autocompleteAVL(raiz, termo, resultados);

            char resposta[512];
            strcpy(resposta,
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "\r\n");

            strcat(resposta, "{ \"resultados\": [");
            for (int i = 0; i < qtd; i++)
            {
                strcat(resposta, "\"");
                strcat(resposta, resultados[i]);
                strcat(resposta, "\"");
                if (i < qtd - 1)
                    strcat(resposta, ",");
            }
            strcat(resposta, "] }");

            send(new_socket, resposta, (int)strlen(resposta), 0);
        }
        else if (strncmp(buffer, "GET / HTTP", strlen("GET / HTTP")) == 0)
        {
            send_file(new_socket, "Front-end/index.html");
        }
        else if (strncmp(buffer, "GET /test", strlen("GET /test")) == 0)
        {
            send_file(new_socket, "Front-end/test.html");
        }
        else if (strncmp(buffer, "GET /chat", strlen("GET /chat")) == 0)
        {
            send_file(new_socket, "Front-end/chat.html");
        }
        else if (strncmp(buffer, "GET /fruits", strlen("GET /fruits")) == 0)
        {
            send_file(new_socket, "Front-end/fruits.html");
        }
        else
        {
            // fallback
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
