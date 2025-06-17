#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#pragma comment(lib, "ws2_32.lib")
#define SA struct sockaddr
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
int count_processes()
{
    int count = 0;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("Error al obtener la lista de procesos.\n");
        return 1;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            count++;
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return count;
}

int scan_port(const char *ip, int port)
{
    // Crear socket y verificar
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("No se pudo crear el socket.\n");
        WSACleanup();
        return 1;
    }
    struct sockaddr_in server;
    // Configurar estructura de conexión
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    //  connect es la funcion q se usa para conectar al servidor remoto
    //  sock es el socket q se va a usar para conectar
    //  server es la estructura q contiene la direccion a la q se va a conectar, el cast es necesario pq el metodo requiere un puntero
    //  el sizeof se usa para saber cuanto espacio va a ocupar la estructura server
    //  Intentar conectar
    /// AKI DA BERRO Y EL CONNECT ESE SE DEMORA CANTIDAD
    printf("Ver q se puede hacer para optimizar");

    // Optimizacion para q cuando el puerto este cerrado se bloquee antes
    DWORD timeout = 500; // 500ms
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
    {
        printf("De aqui alante no he probado");
        int processes_before = count_processes(); // Obtener cantidad de procesos antes
        closesocket(sock);
        WSACleanup();
        int processes_after = count_processes(); // Obtener cantidad de procesos después
        int difference = processes_after - processes_before;
        return 1; // Puerto abierto
    }
    closesocket(sock);
    return 0; // Puerto cerrado
}
/*
int create_server(const char *ip, int Port)
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
    scanf("%s", target_ip);

    printf("Escriba el puerto desde q empieza\n");
    int start_port = 80;
    scanf("%d", &start_port);

    printf("Escriba el puerto en el q termina\n");
    int end_port = 100;
    scanf("%d", &end_port);

    if (start_port > end_port)
    {
        printf("El puerto desde el q empieza debe ser menor q en el q termina\n");
        return 1;
    }
    for (int port = start_port; port <= end_port; port++)
    {
        /*         if (port <= 1024)
                {
                    printf("El puerto \d se usa para procesos reservados del sistema no debe ser peligroso \n", port);
                    continue;
                }
         */
        if (!scan_port(target_ip, port))
            printf("Puerto %d está abierto\n", port);
        else
            printf("Puerto %d está cerrado\n", port);
    }

    printf("Termino de ejecutarse la consulta a los puertos\n");
    WSACleanup();
    return 0;
}
