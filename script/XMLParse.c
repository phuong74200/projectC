#include <stdio.h>
#include <string.h>

char *getXMLValue(char *tag, char *XMLBuffer, char *tagValue) {
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

    int openTagPos = 0;

    for (int i = 0; i < bufferLen; i++) {
        int cmp = memcmp(openTag, XMLBuffer + i, strlen(openTag));
        if (cmp == 0) {
            openTagPos = i;
            break;
        }
    }

    int closeTagPos = 0;

    for (int i = 0; i < bufferLen; i++) {
        int cmp = memcmp(closeTag, XMLBuffer + i, strlen(closeTag));
        if (cmp == 0) {
            closeTagPos = i;
            break;
        }
    }

    memset(tagValue, 0, sizeof(tagValue));

    int startPoint = XMLBuffer + openTagPos + strlen(openTag);
    int endPoint = closeTagPos - openTagPos - strlen(closeTag) + 1;

    memcpy(tagValue, startPoint, endPoint);

    return 0;
}

int main() {
    char res[100];
    getXMLValue("h1", "<h1>Hello</h1><fuong>ProjectCUsername</fuong>", &res);
    printf("%s", res);
}