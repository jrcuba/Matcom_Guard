#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>

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

const char *get_service(int port)
{
    for (int i = 0; i < sizeof(port_services) / sizeof(port_services[0]); i++)
    {
        if (port_services[i].port == port)
        {
            return port_services[i].service;
        }
    }
    return "Desconocido";
}

int count_processes()
{
    int count = 0;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("Error al obtener la lista de procesos.\n");
        return -1;
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
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Error en WSAStartup.\n");
        return 0;
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("No se pudo crear el socket.\n");
        WSACleanup();
        return 0;
    }

    // Configurar estructura de conexión
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Intentar conectar
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
    {
        int processes_before = count_processes(); // Obtener cantidad de procesos antes
        closesocket(sock);
        WSACleanup();
        int processes_after = count_processes(); // Obtener cantidad de procesos después
        int difference = processes_after - processes_before;
        printf("Puerto %d (%s) está abierto.\n", port, get_service(port));
        printf("Diferencia en cantidad de procesos tras cerrar el puerto %d: %d\n", port, difference);
        return 1; // Puerto abierto
    }

    closesocket(sock);
    WSACleanup();
    return 0; // Puerto cerrado
}

int main()
{
    const char *target_ip = "127.0.0.1"; // Cambia por la IP objetivo
    int start_port = 80, end_port = 100; // Define el rango

    for (int port = start_port; port <= end_port; port++)
    {
        if (scan_port(target_ip, port))
        {
            printf("Puerto %d está abierto\n", port);
        }
        else
        {
            printf("Puerto %d está cerrado\n", port);
        }
    }

    return 0;
}
