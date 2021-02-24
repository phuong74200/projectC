#include <dirent.h>
#include <stdio.h>
#include <winsock2.h>

#include "admin.c"
#include "ulities.c"

#define PORT 8080

void slog(char *content) {
    printf("[server]: %s.\n", content);
}

int isAdmin(char token[100]) {
    char filePath[100] = "database\\token_index\\";

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

                    char filePath[100] = "database\\user_index\\";
                    char filePath1[100] = "database\\token_index\\";

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
                        DIR *d;
                        struct dirent *dir;
                        d = opendir("database\\accounts");
                        int count = -2;
                        if (d) {
                            while ((dir = readdir(d)) != NULL) {
                                count++;
                                printf("%d\n", count);
                            }
                            closedir(d);
                        }

                        char bufferCount[100] = "";
                        char filePath3[100] = "database\\accounts\\";
                        itoa(count, bufferCount, 10);
                        strcat(filePath3, bufferCount);
                        strcat(filePath3, ".xml");

                        strcat(userdata, "<index>");
                        strcat(userdata, bufferCount);
                        strcat(userdata, "</index>");

                        printf("buffer of count: %s", filePath3);

                        writeFile(filePath3, userdata);
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

                    char filePath[100] = "database\\user_index\\";
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
                if (strcmp(reqApi, "deleteUser") == 0) {
                    char token[100] = "";
                    parseXML("token", reqBuffer, &token);
                    char username[100] = "";
                    parseXML("username", reqBuffer, &username);
                    if (isAdmin(token) == 1) {
                        printf("token accepted\n");
                        char filePath[100] = "database\\user_index\\";
                        strcat(filePath, username);
                        strcat(filePath, ".xml");
                        if (checkFile(filePath) == 1) {
                            char userdata[1000] = "";
                            readFile(filePath, &userdata);

                            char uindex[100] = "";
                            char uname[100] = "";
                            char utoken[100] = "";

                            char findex[1000] = "database\\accounts\\";
                            char fname[1000] = "database\\user_index\\";
                            char ftoken[1000] = "database\\token_index\\";

                            parseXML("index", userdata, &uindex);
                            parseXML("username", userdata, &uname);
                            parseXML("token", userdata, &utoken);

                            strcat(findex, uindex);
                            strcat(findex, ".xml");

                            strcat(fname, uname);
                            strcat(fname, ".xml");

                            strcat(ftoken, utoken);
                            strcat(ftoken, ".xml");

                            remove(findex);
                            remove(fname);
                            remove(ftoken);

                            printf("%s\n", findex);
                            printf("%s\n", fname);
                            printf("%s\n", ftoken);
                        }
                    } else {
                        printf("%s -- token rejected\n", token);
                    }
                    closesocket(newReq);
                }

                if (strcmp(reqApi, "switchRole") == 0) {
                    char token[100] = "";
                    parseXML("token", reqBuffer, &token);
                    char username[100] = "";
                    parseXML("username", reqBuffer, &username);
                    if (isAdmin(token) == 1) {
                        printf("token accepted\n");
                        char filePath[100] = "database\\user_index\\";
                        strcat(filePath, username);
                        strcat(filePath, ".xml");
                        if (checkFile(filePath) == 1) {
                            char userdata[1000] = "";
                            readFile(filePath, &userdata);

                            char uindex[100] = "";
                            char uname[100] = "";
                            char utoken[100] = "";
                            char upass[100] = "";
                            char urole[100] = "";

                            char findex[1000] = "database\\accounts\\";
                            char fname[1000] = "database\\user_index\\";
                            char ftoken[1000] = "database\\token_index\\";

                            parseXML("index", userdata, &uindex);
                            parseXML("username", userdata, &uname);
                            parseXML("token", userdata, &utoken);
                            parseXML("password", userdata, &upass);
                            parseXML("role", userdata, &urole);

                            strcat(findex, uindex);
                            strcat(findex, ".xml");

                            strcat(fname, uname);
                            strcat(fname, ".xml");

                            strcat(ftoken, utoken);
                            strcat(ftoken, ".xml");

                            char newData[1000] = "<username>";

                            strcat(newData, uname);
                            strcat(newData, "</username><password>");
                            strcat(newData, upass);
                            strcat(newData, "</password><token>");
                            strcat(newData, utoken);
                            strcat(newData, "</token><role>");

                            if (strcmp(urole, "admin") == 0) {
                                strcat(newData, "user</role><index>");
                            } else {
                                strcat(newData, "admin</role><index>");
                            }

                            strcat(newData, uindex);
                            strcat(newData, "index");

                            writeFile(findex, newData);
                            writeFile(fname, newData);
                            writeFile(ftoken, newData);

                            printf("%s\n", findex);
                            printf("%s\n", fname);
                            printf("%s\n", ftoken);
                        }
                    } else {
                        printf("%s -- token rejected\n", token);
                    }
                    closesocket(newReq);
                }

                if (strcmp(reqApi, "getUserList") == 0) {
                    int count = 0;

                    char token[100] = "";
                    parseXML("token", reqBuffer, &token);

                    if (isAdmin(token) == 1) {
                        char index[100] = "";
                        parseXML("index", reqBuffer, &index);

                        char userdata[200] = "";
                        char filePath[200] = "database\\accounts\\";
                        strcat(filePath, index);
                        strcat(filePath, ".xml");

                        if (checkFile(filePath) == 1) {
                            readFile(filePath, &userdata);
                            printf("%s\n", userdata);
                            send(newReq, userdata, strlen(userdata), 0);
                        } else {
                            char none[] = "<username>(none)</username><password>(none)</password><token>(none)</token><role>(none)</role>";
                            send(newReq, none, strlen(none), 0);
                        }
                        printf("%s\n", filePath);
                    } else {
                        send(newReq, "no_permisson", 100, 0);
                    }
                    closesocket(newReq);
                }

                if (strcmp(reqApi, "certificate") == 0) {
                    char filePath[100] = "database\\token_index\\";
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
                    printf("token result: %s\n", request);
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
