#include <stdio.h>
#include <winsock2.h>
#include "ulities.c"

#define PORT 8080

#define ADDR "192.168.1.10"

void slog(char *content) {
    printf("[client]: %s.\n", content);
}

char *_request(char message[200]) {
    WSADATA wsaData;

    SOCKET initSocket;

    int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (initResult != 0) {
        return "WSA_Failed";
    }

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

    static char response[200];

    recv(initSocket, &response, 200, 0);

    closesocket(initSocket);

    return response;
}

int main() {
    char message[200];
    char *response;
    while (1) {
        slog("init socket done");
        scanf("%s", &message);
        response = _request(message);
    
        char itemName[100];
        parseXML("name", response, &itemName);

        printf("%s\n", itemName);
    }
}