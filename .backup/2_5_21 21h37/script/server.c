#include <stdio.h>
#include <winsock2.h>

#include "ulities.c"

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

    // socket(adress, <Ipv4 or Ipv6>, protocol)

    // protocol equal to 0 means that it let service provider to choose

    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    slog("init socket");
    if (initSocket == INVALID_SOCKET) {
        slog("Can't init socket");
    } else {
        slog("init socket success");
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;

    // don't bind socket to a specfic IP

    address.sin_port = htons(PORT);

    // inet_addr("127.0.0.1")

    address.sin_addr.s_addr = INADDR_ANY;

    bind(initSocket, (struct sockaddr *)&address, sizeof(address));

    printf("[server]: listening: %s:%d\n", inet_ntoa(address.sin_addr), PORT);
    listen(initSocket, 5);

    char reqBuffer[1024];

    char res[] = "response";

    while (1) {
        int socketAddrSize = sizeof(struct sockaddr);

        SOCKET newReq = accept(initSocket, (struct sockaddr *)&address, &socketAddrSize);

        if (newReq == -1) {
            slog("Client connect error");
        }

        setColor(1);
        printf("\n\n[reqs-accept]: Recive request from %s\n\n", inet_ntoa(address.sin_addr));
        clearColor();

        memset(reqBuffer, 0, sizeof(reqBuffer));

        recv(newReq, reqBuffer, sizeof(reqBuffer), 0);

        // send(newReq, header, sizeof(header), 0);
        // send(newReq, body, sizeof(body), 0);

        // send(newReq, res, sizeof(res), 0);

        printf("[req-content]: %s\n", reqBuffer);

        // Write Events following

        reqBuffer[strlen(reqBuffer)];

        char reqApi[200];

        parseXML("api", reqBuffer, &reqApi);
        printf("[request-api]: %s\n", reqApi);

        if (strcmp(reqApi, "getProducts") == 0) {
            printf("accepted\n");
            char products[100];
            getFile("database\\products\\sp1.xml", &products);
            send(newReq, products, sizeof(products), 0);
            closesocket(newReq);
        }

        send(newReq, "event not available...", 300, 0);
        closesocket(newReq);
    }

    return 0;
}