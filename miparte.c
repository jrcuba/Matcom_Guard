#include<winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(80); // Prueba con puerto 80 (HTTP)

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
    {
        printf("¡Conexión exitosa! El puerto está abierto.\n");
    }
    else
    {
        printf("Error al conectar: %d\n", WSAGetLastError());
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}