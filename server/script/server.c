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

    // don't bind socket to any specfic IP

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

        char reqApi[200] = "";

        parseXML("api", reqBuffer, &reqApi);
        printf("[request-api]: %s\n", reqApi);

        // ------ Phuong ------

        if (strcmp(reqApi, "getProducts") == 0) {
            printf("accepted\n");
            char products[100] = "";
            char id[100] = "";

            parseXML("id", reqBuffer, &id);
            strcat(id, ".xml");
            char filePath[100] = "database\\products\\";
            strcat(filePath, id);
            readFile(filePath, &products);
            //printf("%s", reqBuffer);
            send(newReq, products, sizeof(products), 0);
            closesocket(newReq);
        }

        if (strcmp(reqApi, "login") == 0) {
            printf("accepted\n");

            char accountData[1000] = "";
            char username[99] = "";
            char password[99] = "";

            parseXML("username", reqBuffer, &username);
            parseXML("password", reqBuffer, &password);

            char filePath[100] = "database\\accounts\\";
            strcat(filePath, username);
            strcat(filePath, ".xml");
            readFile(filePath, &accountData);

            printf("reqBuffer: %s\n", reqBuffer);
            printf("database: %s\n", accountData);
            printf("filepath: %s\n", filePath);
            printf("username: %s\n", username);
            printf("password: %s\n", password);

            char truePassword[40] = "";
            parseXML("password", accountData, &truePassword);
            char trueUsername[40] = "";
            parseXML("username", accountData, &trueUsername);

            printf("true_username: %s\n", trueUsername);
            printf("true_password: %s\n", truePassword);

            char status[40] = "login_fail";

            printf("%d\n", strcmp(password, truePassword));
            printf("%d\n", strcmp(username, trueUsername));

            if (strcmp(password, truePassword) == 0 && strcmp(username, trueUsername) == 0) {
                memset(status, 0, sizeof(status));
                strcpy(status, "login_success");
            }

            send(newReq, status, sizeof(status), 0);
            closesocket(newReq);
        }

        // ------ Phuong ------

        if (strcmp(reqApi, "inputNewProducts") == 0) {
            printf("accepted\n");
            char id[100] = "";

            parseXML("id", reqBuffer, &id);
            strcat(id, ".xml");
            char filePath[100] = "database\\products\\";
            strcat(filePath, id);
            writeFile(filePath, reqBuffer + 28);
            closesocket(newReq);
        }
        if (strcmp(reqApi, "deleteProducts") == 0) {
            printf("accepted\n");
            char id[100] = "";
            parseXML("id", reqBuffer, &id);
            strcat(id, ".xml");
            char filePath[100] = "database\\products\\";
            strcat(filePath, id);
            remove(filePath);
            closesocket(newReq);
        }

        send(newReq, "event not available...", 300, 0);
        closesocket(newReq);
    }

    return 0;
}
