#include <stdio.h>
#include <winsock2.h>

#include "admin.c"
#include "ulities.c"

#define PORT 8080

void slog(char *content) {
    printf("[server]: %s.\n", content);
}

int isAdmin(char token[100]) {
    char filePath[100] = "";

    strcat(filePath, token);
    strcat(filePath, ".xml");

    char accountData[2000] = "";

    if (checkFile(filePath) == 1) {
        readFile(filePath, &accountData);
        char role[100] = "";
        parseXML("role", accountData, &role);
        if (strcmp(role, "admin") == 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
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

    struct product item1;

    strcpy(item1.name, "kaki");
    item1.id = 1;
    item1.price = 2;
    item1.quantity = 3;
    item1.price = 4;
    item1.quantity = 5;
    item1.soldQuantity = 6;

    srand(time(NULL));

    while (1) {
        int socketAddrSize = sizeof(struct sockaddr);

        SOCKET newReq = accept(initSocket, (struct sockaddr *)&address, &socketAddrSize);

        if (newReq == -1) {
            slog("Client connect error");
        }

        setColor(1);
        printf("\n\n[reqs-accept]: Receive request from %s\n\n", inet_ntoa(address.sin_addr));
        clearColor();

        memset(reqBuffer, 0, sizeof(reqBuffer));

        DWORD timeout = 1 * 1000;
        setsockopt(newReq, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        int recv_size = recv(newReq, reqBuffer, sizeof(reqBuffer), 0);

        // send(newReq, header, sizeof(header), 0);
        // send(newReq, body, sizeof(body), 0);

        // send(newReq, res, sizeof(res), 0);

        if (recv_size != SOCKET_ERROR) {
            if (WSAGetLastError() != WSAETIMEDOUT) {
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

                if (strcmp(reqApi, "register") == 0) {
                    printf("accepted\n");
                    char accountData[1000] = "";
                    char username[99] = "";
                    char password[99] = "";
                    char userdata[1000] = "";
                    char response[200] = "";

                    char token[30] = "";
                    strcpy(token, randstring(30));

                    parseXML("username", reqBuffer, &username);
                    parseXML("password", reqBuffer, &password);

                    char filePath[100] = "database\\accounts\\";
                    char filePath1[100] = "database\\accounts\\";

                    strcat(filePath, username);
                    strcat(filePath, ".xml");

                    strcat(filePath1, token);
                    strcat(filePath1, ".xml");

                    strcat(userdata, "<username>");
                    strcat(userdata, username);
                    strcat(userdata, "</username><password>");
                    strcat(userdata, password);
                    strcat(userdata, "</password><token>");
                    strcat(userdata, token);
                    strcat(userdata, "</token><role>user</role>");

                    if (checkFile(filePath) == 0) {
                        writeFile(filePath, userdata);
                        writeFile(filePath1, userdata);
                        strcat(response, "success");
                    } else {
                        strcat(response, "existed");
                    }

                    send(newReq, response, sizeof(response), 0);
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

                    char status[300] = "login_fail";

                    if (checkFile(filePath) == 1) {
                        printf("%d\n", strcmp(password, truePassword));
                        printf("%d\n", strcmp(username, trueUsername));
                        if (strcmp(password, truePassword) == 0 && strcmp(username, trueUsername) == 0) {
                            memset(status, 0, sizeof(status));
                            char fileData[400] = "";
                            readFile(filePath, &fileData);
                            char accToken[40] = "";
                            parseXML("token", fileData, &accToken);
                            strcpy(status, "<status>login_success</status><token>");
                            strcat(status, accToken);
                            strcat(status, "</token>");
                            printf("token: %s\n", status);
                        }
                    } else {
                        memset(status, 0, sizeof(status));
                        strcpy(status, "not_exist");
                    }

                    send(newReq, status, sizeof(status), 0);
                    closesocket(newReq);
                }

                // ------ Phuong ------

                if (strcmp(reqApi, "certificate") == 0) {
                    char filePath[100] = "database\\accounts\\";
                    char request[100] = "not_exist";

                    char userToken[100] = "";
                    parseXML("token", reqBuffer, &userToken);

                    strcat(filePath, userToken);
                    strcat(filePath, ".xml");
                    if (checkFile(filePath) == 1) {
                        char userdata[2000] = "";
                        readFile(filePath, &userdata);
                        char username[200] = "";
                        parseXML("role", userdata, &username);
                        strcpy(request, username);
                        printf("token result: %s\n", username);
                    }
                    send(newReq, request, strlen(request), 0);
                    closesocket(newReq);
                }

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
            } else {
                printf("session timed out!\n");
                closesocket(newReq);
            }
        } else {
            printf("session rejected!\n");
            closesocket(newReq);
        }
    }

    printf("end session\n");

    return 0;
}
