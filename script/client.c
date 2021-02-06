#include <conio.h>
#include <stdio.h>
#include <winsock2.h>

#include "ulities.c"

#define PORT 8080

// server's local IPv4 address

#define ADDR "192.168.1.10"

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

    static char response[200];

    recv(initSocket, &response, 200, 0);

    closesocket(initSocket);

    return response;
}

void itemSlide(x, y) {
}

int main() {
    
    initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    char message[200];
    char *response;

    while (1) {
        scanf("%s", &message);
        response = fetch(message);

        char itemName[100];
        parseXML("name", response, &itemName);

        printf("%s\n", itemName);
    }

    /*
    setWindowSize(1280, 720);

    char fetchData[] = "<api>getProducts</api><spi>sp1</spi>";

    for (int i = 0; i < 10; i++) {
        setColor(i);
        printf("Color %d\n", i);
    }

    printf("%s", fetch(fetchData));

    char keypressed = 'i';

    float refreshRate = 1000 / 60;

    int x = 0;
    int y = 0;

    char graphics[1000][1000] = {
        "000",
        "000",
    };

    HideCursor();
    while (1) {
        for (int i = 0; i < strlen(graphics); i++) {
            clrscr();
            for (int j = 0; j < strlen(graphics[i]); i++) {
                clrscr();
                gotoxy(x + j, y + i);
                setColor(11);
                printf("ooo\n");
                clearColor();
            }
        }

        keypressed = getch();
        if (keypressed == 72) {
            y--;
        }
        if (keypressed == 75) {
            x--;
        }
        if (keypressed == 80) {
            y++;
        }
        if (keypressed == 77) {
            x++;
        }

        if (keypressed) {
            gotoxy(0, 0);
            printf("%d", keypressed);
        }

        Sleep(refreshRate);
    }

    getchar();
    */
}