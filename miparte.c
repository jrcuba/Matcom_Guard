#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int NumberPort;       // number of the port
    char *Name;           // name of the port
    char *RequestFormat;  // format of the request
    char *ResponsiveType; // type of the response expected
} Ports;

void create(Ports Port, int number, char name[], char request[], char responsive[])
{
    Port.NumberPort = number;
    Port.Name = name;
    Port.RequestFormat = request;
    Port.ResponsiveType = responsive;
};

/// RECONOCER TODOS LOS PUERTOS DISPONIBLES Y VER SI ESTAN ABIERTOS
// codigo de linux
/*
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define START_PORT 1   // Inicio del rango de puertos
#define END_PORT 1023  // Fin del rango de puertos

int main() {
    int sockfd;
    struct sockaddr_in addr;

    for (int port = START_PORT; port <= END_PORT; port++) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
         {
            perror("Error creando socket");
            continue;
        }

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        // Intentamos asignar el puerto
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            printf("Puerto %d está disponible y asignado exitosamente\n", port);
        } else {
            printf("Puerto %d ocupado o no accesible\n", port);
        }

        close(sockfd); // Cerramos el socket antes de probar el siguiente puerto
    }

    return 0;
}
*/

/* void ErrorDetector(int n)
{
    if (n == -1)
        perror("Error");
}
 */
// codigo de windows
/*
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Vincula la librería Winsock

#define START_PORT 1  // Inicio del rango de puertos
#define END_PORT 1023 // Fin del rango de puertos

int main()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in addr;

    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Error al inicializar Winsock.\n");
        return 1;
    }

    for (int port = START_PORT; port <= END_PORT; port++)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            printf("Error al crear el socket.\n");
            continue;
        }

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        // Intentamos asignar el puerto
        if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0)
        {
            printf("Puerto %d disponible y asignado exitosamente.\n", port);
        }
        else
        {
            printf("Puerto %d ocupado o no accesible.\n", port);
        }

        closesocket(sock); // Cerramos el socket antes de probar el siguiente puerto
    }

    WSACleanup(); // Limpiamos Winsock
    return 0;
}
 */

/// ASOCIAR LOS PUERTOS CON SERVICIOS COMUNES PARA BUSCAR ANOMALIAS
