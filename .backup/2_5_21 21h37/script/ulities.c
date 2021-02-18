#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int *parseXML(char *tag, char *XMLBuffer, char *tagValue) {
    char openTag[10];
    char closeTag[10];

    memset(openTag, 0, sizeof(openTag));
    strcpy(openTag, "<");
    strcat(openTag, tag);
    strcat(openTag, ">");

    memset(closeTag, 0, sizeof(closeTag));
    strcpy(closeTag, "</");
    strcat(closeTag, tag);
    strcat(closeTag, ">");

    int bufferLen = strlen(XMLBuffer);

    int openTagPos = -1;

    for (int i = 0; i < bufferLen; i++) {
        int cmp = memcmp(openTag, XMLBuffer + i, strlen(openTag));
        if (cmp == 0) {
            openTagPos = i;
            break;
        }
    }

    int closeTagPos = -1;

    for (int i = 0; i < bufferLen; i++) {
        int cmp = memcmp(closeTag, XMLBuffer + i, strlen(closeTag));
        if (cmp == 0) {
            closeTagPos = i;
            break;
        }
    }

    if(openTagPos == -1 || closeTagPos == -1) {
        return 0;
    }

    memset(tagValue, 0, sizeof(tagValue));

    int startPoint = XMLBuffer + openTagPos + strlen(openTag);
    int endPoint = closeTagPos - openTagPos - strlen(closeTag) + 1;

    memcpy(tagValue, startPoint, endPoint);

    return 0;
}

char *getFile(char *dir, char *value) {
    char *buffer = 0;
    long length;
    FILE *f = fopen(dir, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread(value, 1, length, f);
        }
        fclose(f);
    }
}

int hash(char *str) {
    int result = 5381;
    int c, i;
    int len = 100;
    for (result = i = 0; i < len; ++i) {
        result += str[i];
        result += (result << 10);
        result ^= (result >> 6);
    }
    return result;
}

void gotoxy(short x, short y) {
    HANDLE hConsoleOutput;
    COORD Cursor_Pos = {x, y};

    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsoleOutput, Cursor_Pos);
}

void clrscr(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
    SetConsoleCursorPosition(hStdOut, coord);
}

void HideCursor() {
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &cursor);
}

void setWindowSize(int col, int lin) {
    DWORD sWidth = GetSystemMetrics(SM_CXSCREEN);
    DWORD sHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = GetConsoleWindow();
    RECT rect = {sWidth / 2 - col / 2, sHeight / 2 - lin / 2, col, lin};
    MoveWindow(hwnd, rect.left, rect.top, rect.right, rect.bottom, TRUE);
}

void setColor(int colorCode) {
    /*
        0 Black
        1 Blue
        2 Green
        3 Cyan
        4 Red
        5 Magenta
        6 Yellow
        7 White
        8 Grey
    */

    int color;
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

void clearColor() {
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

/*
int main() {
    char res[100];
    getXMLValue("h1", "<h1>Hello</h1><h1>fuong</h1><fuong>ProjectCUsername</fuong>", &res);
    printf("%s\n", res);

    char fileData[200];
    getFile("script\\client.c", &fileData);
    printf("%s", fileData);
    getchar();
}
*/