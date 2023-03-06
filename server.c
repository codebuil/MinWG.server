#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define DEFAULT_PORT 8010

void handle_request(SOCKET client_socket) {
    char buffer[BUF_SIZE];
    int recv_bytes;
    char response[BUF_SIZE];
printf("hello world\r\n");
    // Construct HTTP response
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\nHello, world!\r\n", strlen("Hello, world!\r\n"));

    // Send HTTP response
    send(client_socket, response, strlen(response), 0);
}

int main() {
    WSADATA wsaData;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error initializing Winsock.\n");
        return 1;
    }

    // Create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        printf("Error creating socket.\n");
        WSACleanup();
        return 1;
    }

    // Bind socket to port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(DEFAULT_PORT);
    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error binding socket to port.\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Error listening on socket.\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    printf("\033c\033[44;30mServer running on port %d...\n", DEFAULT_PORT);

    // Loop to accept and handle connections
    while (1) {
        // Accept incoming connection
        client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Error accepting connection.\n");
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        }

        printf("Client connected.\n");

        // Handle request
        handle_request(client_socket);

        // Cleanup
        closesocket(client_socket);
    }

    // Cleanup
    closesocket(listen_socket);
    WSACleanup();

    return 0;
}