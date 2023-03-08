#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 5000000
#define DEFAULT_PORT 8012
char *buffers;
char *response;
#include <stdio.h>
#include <string.h>

const char* get_content_type(const char* filename) {
    const char* extension = strchr(filename, '.');
    if (extension == NULL) {
        return "application/octet-stream";
    } else if (strcmp(extension, ".html") == 0) {
        return "text/html";
    } else if (strcmp(extension, ".css") == 0) {
        return "text/css";
    } else if (strcmp(extension, ".js") == 0) {
        return "text/javascript";
    } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(extension, ".png") == 0) {
        return "image/png";
    } else if (strcmp(extension, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(extension, ".pdf") == 0) {
        return "application/pdf";
    } else {
        return "application/octet-stream";
    }
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *cdest = dest;
    const char *csrc = src;
    size_t i;
    for (i = 0; i < n; i++)
        cdest[i] = csrc[i];
    return dest;
}

void handle_request(SOCKET client_socket,char *c) {
    char *p1;    
    int recv_bytes;
    char buf[1024];
    printf("call\r\n");
    int lenl=0; 
    int lenl2=0;
    FILE *fp;
    fp = fopen(c, "r");
    buffers[0]=0;
    if(fp!=NULL){       
        lenl=fread(buffers, sizeof(char), BUF_SIZE-1024, fp);
        buffers[BUF_SIZE-1024]=0;
        fclose(fp);
        // Construct HTTP response
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: ");   
        strcat(response,get_content_type(c));
        sprintf(buf, "\r\nContent-Length: %d\r\n\r\n", lenl);
        strcat(response,buf);
        lenl2=strlen(response);
        p1=response+lenl2;
        memcpy(p1,buffers,lenl);
        // Send HTTP response
        send(client_socket, response, lenl2+lenl, 0);
    }
    else{
       sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n\r\n\r\n"); 
       send(client_socket, response, strlen(response), 0);
    }
}

int main() {
    char buffer[4096];
    WSADATA wsaData;
    char *ptr;
    char *ptr2;
    char *ptr3;
    int recv_bytes;
    SOCKET listen_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    buffers = (char*) malloc(BUF_SIZE);
    response = (char*) malloc(BUF_SIZE);
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
        ptr=NULL;
        ptr2=NULL;
        int received = recv(client_socket, buffer, sizeof(buffer), 0);
        ptr = strchr(buffer, '/');
        if(ptr!=NULL){
            
            ptr2 = strchr(ptr,' ');
            if(ptr2!=NULL){
                
                ptr2[0]=0;
                ptr++;
                printf("Received request %s\n", ptr);
                handle_request(client_socket,ptr);
                
                
            }
        }
        // Handle request
    
       
    }

    // Cleanup
    closesocket(listen_socket);
    WSACleanup();
    free(response);
    free(buffers);

    return 0;
}
