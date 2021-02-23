#include <conio.h>
#include <stdio.h>
#include <winsock2.h>

#define PORT 8080

// server's local IPv4 address

#define ADDR "192.168.1.14"

void slog(char *content) {
    printf("[client]: %s.\n", content);
}

WSADATA wsaData;

SOCKET initSocket;

int initResult;

char *fetch(char message[200]) {
    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (initSocket == INVALID_SOCKET) {
        return "init_socket_failed";
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }

    send(initSocket, message, strlen(message), 0);
}

int main() {
    initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    char *response = "";
    response = fetch("</api>fuong</api>");
    getchar();
}