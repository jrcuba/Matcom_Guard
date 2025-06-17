#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
// Estructura para mapear puertos a servicios
typedef struct
{
    int port;
    const char *service;
} PortService;

// Lista de puertos comunes y sus servicios
PortService port_services[] = {
    {80, "HTTP"},
    {443, "HTTPS"},
    {21, "FTP"},
    {22, "SSH"},
    {25, "SMTP"},
    {53, "DNS"},
    {110, "POP3"},
    {143, "IMAP"},
    {3306, "MySQL"},
    {3389, "RDP"},
    {8080, "HTTP-Proxy"}};

// Escanea los servicios comunes guardados a ver si el puerto en cuestion coincide con alguno de estos servicios guardados
const char *get_service(int port)
{
    for (int i = 0; i < (int)(sizeof(port_services) / sizeof(port_services[0])); i++)
        if (port_services[i].port == port)
            return port_services[i].service;
    return "Desconocido";
}

// Metodo para saber cuantos procesos se ejecutan mientra el puerto esta abierto
// Si un proceso tiene muchos procesos abiertos es probable q sea un puerto peligroso

int count_processes(DWORD target_port)
{
    PMIB_TCPTABLE tcp_table = NULL;
    DWORD size = 0;
    int count = 0;

    if (GetTcpTable(NULL, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER)
    {
        tcp_table = (PMIB_TCPTABLE)malloc(size);
        if (GetTcpTable(tcp_table, &size, TRUE) == NO_ERROR)
            for (DWORD i = 0; i < tcp_table->dwNumEntries; i++)
                if (ntohs(tcp_table->table[i].dwLocalPort) == target_port)
                    count++;

        free(tcp_table);
    }
    return count;
}
int scan_port(const char *ip, int port)
{
    // Crear socket y verificar
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("No se pudo crear el socket.\n");
        return -1;
    }
    struct sockaddr_in server;
    // Configurar estructura de conexión a la q se va a conectar el socket
    server.sin_family = AF_INET;            // IPv4
    server.sin_addr.s_addr = inet_addr(ip); // ip en formato de red para leer a.b.c.d
    server.sin_port = htons(port);          // Puerto en formato de red para q lo lea la computadora

    // Optimizacion para q cuando el puerto este cerrado se bloquee antes
    DWORD timeout = 2000; // 500ms
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    int processes_before = count_processes(port); // Obtener cantidad de procesos antes de conectar con el puerto

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
    {
        printf("De aqui alante no he probado");
        closesocket(sock);
        int processes_after = count_processes(port); // Obtener cantidad de procesos después de conectar con el peurto
        printf("Puerto :%d abierto y ejecuta %d procesos", port, processes_before - processes_after);
        return 1; // Puerto abierto
    }

    closesocket(sock);
    printf("Puerto %d: CERRADO\n", port);
    return 0; // Puerto cerrado
}

int main()
{
    // Inicializar Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Error en WSAStartup.\n");
        return 1;
    }

    printf("Hola\n");
    printf("Escriba el ip al q se va a conectar el server\n");
    const char *target_ip = malloc(sizeof(char) * 16);
    int start_port = 80;
    int end_port = 100;

    printf("Ingrese la IP objetivo (ej: 192.168.1.1): \n");
    scanf("%15s", target_ip);
    printf("Ingrese puerto inicial (ej: 80): \n");
    scanf("%d", &start_port);
    printf("Ingrese puerto final (ej: 100): \n");
    scanf("%d", &end_port);

    if (start_port > end_port)
    {
        printf("El puerto desde el q empieza debe ser menor q en el q termina\n");
        return 1;
    }
    for (int port = start_port; port <= end_port; port++)
        scan_port(target_ip, port);

    printf("Termino de ejecutarse la consulta a los puertos\n");
    WSACleanup();
    return 0;
}

/* int create_server(const char *ip, int Port)
{
    int sock, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Crear socket y verificar
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != 0)
    {
        printf("socket creation failed...\n");
        return 1;
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assignary Protocolo a usar, IP y PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(Port);

    // Binding newly created socket to given IP and verification
    if ((bind(sock, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        return 1;
    }
    else
        printf("Socket successfully binded..\n");

    // Aki el server esta escuchando y se puede verificar
    if ((listen(sock, 5)) != 0)
    {
        printf("Listen failed...\n");
        return 1;
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Verificando datos recibidos
    connfd = accept(sock, (SA *)&cli, &len);
    if (connfd != 0)
    {
        printf("server accept failed...\n");
        return 1;
    }
    else
        printf("server accept the client...\n");

    close(sock);
    return 0;
}
 */
