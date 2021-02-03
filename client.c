#include <stdio.h>
#include <winsock2.h>

#define PORT 8080

void slog(char *content) {
    printf("[server]: %s.\n", content);
}

int main() {
    WSADATA wsaData;

    SOCKET initSocket;

    int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    slog("init winsock");

    if (initResult != 0) {
        slog("WSAStartup failed");
        slog("init winsock failed");
        return 0;
    } else {
        slog("init winsock success");
    }

    // AF_INIT is IPv4, AF_IPX is Ipv6

    // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket

    //socket(adress, <Ipv4 or Ipv6>, protocol)

    //protocol equal to 0 means that it let service provider to choose

    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    slog("init socket");
    if (initSocket == INVALID_SOCKET) {
        slog("Can't init socket");
    } else {
        slog("init socket success");
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    slog("Connectting to server");

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        slog("connect failed");
    } else {
        slog("Connect success");
    }

    char message[] = "fuong";

    send(initSocket, message, strlen(message), 0);


    getchar();
    return 0;
}